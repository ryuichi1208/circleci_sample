int
main (int argc, char **argv)
{
  int i;
  struct pending *thispend;
  int n_files;

  /* The signals that are trapped, and the number of such signals.  */
  static int const sig[] =
    {
      /* This one is handled specially.  */
      SIGTSTP,

      /* The usual suspects.  */
      SIGALRM, SIGHUP, SIGINT, SIGPIPE, SIGQUIT, SIGTERM,
#ifdef SIGPOLL
      SIGPOLL,
#endif
#ifdef SIGPROF
      SIGPROF,
#endif
#ifdef SIGVTALRM
      SIGVTALRM,
#endif
#ifdef SIGXCPU
      SIGXCPU,
#endif
#ifdef SIGXFSZ
      SIGXFSZ,
#endif
    };
  enum { nsigs = ARRAY_CARDINALITY (sig) };

#if ! SA_NOCLDSTOP
  bool caught_sig[nsigs];
#endif

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (LS_FAILURE);
  atexit (close_stdout);

  assert (ARRAY_CARDINALITY (color_indicator) + 1
          == ARRAY_CARDINALITY (indicator_name));

  exit_status = EXIT_SUCCESS;
  print_dir_name = true;
  pending_dirs = NULL;

  current_time.tv_sec = TYPE_MINIMUM (time_t);
  current_time.tv_nsec = -1;

  i = decode_switches (argc, argv);

  if (print_with_color)
    parse_ls_color ();

  /* Test print_with_color again, because the call to parse_ls_color
     may have just reset it -- e.g., if LS_COLORS is invalid.  */
  if (print_with_color)
    {
      /* Avoid following symbolic links when possible.  */
      if (is_colored (C_ORPHAN)
          || (is_colored (C_EXEC) && color_symlink_as_referent)
          || (is_colored (C_MISSING) && format == long_format))
        check_symlink_color = true;

      /* If the standard output is a controlling terminal, watch out
         for signals, so that the colors can be restored to the
         default state if "ls" is suspended or interrupted.  */

      if (0 <= tcgetpgrp (STDOUT_FILENO))
        {
          int j;
#if SA_NOCLDSTOP
          struct sigaction act;

          sigemptyset (&caught_signals);
          for (j = 0; j < nsigs; j++)
            {
              sigaction (sig[j], NULL, &act);
              if (act.sa_handler != SIG_IGN)
                sigaddset (&caught_signals, sig[j]);
            }

          act.sa_mask = caught_signals;
          act.sa_flags = SA_RESTART;

          for (j = 0; j < nsigs; j++)
            if (sigismember (&caught_signals, sig[j]))
              {
                act.sa_handler = sig[j] == SIGTSTP ? stophandler : sighandler;
                sigaction (sig[j], &act, NULL);
              }
#else
          for (j = 0; j < nsigs; j++)
            {
              caught_sig[j] = (signal (sig[j], SIG_IGN) != SIG_IGN);
              if (caught_sig[j])
                {
                  signal (sig[j], sig[j] == SIGTSTP ? stophandler : sighandler);
                  siginterrupt (sig[j], 0);
                }
            }
#endif
        }
    }

  if (dereference == DEREF_UNDEFINED)
    dereference = ((immediate_dirs
                    || indicator_style == classify
                    || format == long_format)
                   ? DEREF_NEVER
                   : DEREF_COMMAND_LINE_SYMLINK_TO_DIR);

  /* When using -R, initialize a data structure we'll use to
     detect any directory cycles.  */
  if (recursive)
    {
      active_dir_set = hash_initialize (INITIAL_TABLE_SIZE, NULL,
                                        dev_ino_hash,
                                        dev_ino_compare,
                                        dev_ino_free);
      if (active_dir_set == NULL)
        xalloc_die ();

      obstack_init (&dev_ino_obstack);
    }

  format_needs_stat = sort_type == sort_time || sort_type == sort_size
    || format == long_format
    || print_scontext
    || print_block_size;
  format_needs_type = (! format_needs_stat
                       && (recursive
                           || print_with_color
                           || indicator_style != none
                           || directories_first));

  if (dired)
    {
      obstack_init (&dired_obstack);
      obstack_init (&subdired_obstack);
    }

  cwd_n_alloc = 100;
  cwd_file = xnmalloc (cwd_n_alloc, sizeof *cwd_file);
  cwd_n_used = 0;

  clear_files ();

  n_files = argc - i;

  if (n_files <= 0)
    {
      if (immediate_dirs)
        gobble_file (".", directory, NOT_AN_INODE_NUMBER, true, "");
      else
        queue_directory (".", NULL, true);
    }
  else
    do
      gobble_file (argv[i++], unknown, NOT_AN_INODE_NUMBER, true, "");
    while (i < argc);

  if (cwd_n_used)
    {
      sort_files ();
      if (!immediate_dirs)
        extract_dirs_from_files (NULL, true);
      /* `cwd_n_used' might be zero now.  */
    }

  /* In the following if/else blocks, it is sufficient to test `pending_dirs'
     (and not pending_dirs->name) because there may be no markers in the queue
     at this point.  A marker may be enqueued when extract_dirs_from_files is
     called with a non-empty string or via print_dir.  */
  if (cwd_n_used)
    {
      print_current_files ();
      if (pending_dirs)
        DIRED_PUTCHAR ('\n');
    }
  else if (n_files <= 1 && pending_dirs && pending_dirs->next == 0)
    print_dir_name = false;

  while (pending_dirs)
    {
      thispend = pending_dirs;
      pending_dirs = pending_dirs->next;

      if (LOOP_DETECT)
        {
          if (thispend->name == NULL)
            {
              /* thispend->name == NULL means this is a marker entry
                 indicating we've finished processing the directory.
                 Use its dev/ino numbers to remove the corresponding
                 entry from the active_dir_set hash table.  */
              struct dev_ino di = dev_ino_pop ();
              struct dev_ino *found = hash_delete (active_dir_set, &di);
              /* ASSERT_MATCHING_DEV_INO (thispend->realname, di); */
              assert (found);
              dev_ino_free (found);
              free_pending_ent (thispend);
              continue;
            }
        }

      print_dir (thispend->name, thispend->realname,
                 thispend->command_line_arg);

      free_pending_ent (thispend);
      print_dir_name = true;
    }

  if (print_with_color)
    {
      int j;

      if (used_color)
        {
          /* Skip the restore when it would be a no-op, i.e.,
             when left is "\033[" and right is "m".  */
          if (!(color_indicator[C_LEFT].len == 2
                && memcmp (color_indicator[C_LEFT].string, "\033[", 2) == 0
                && color_indicator[C_RIGHT].len == 1
                && color_indicator[C_RIGHT].string[0] == 'm'))
            restore_default_color ();
        }
      fflush (stdout);

      /* Restore the default signal handling.  */
#if SA_NOCLDSTOP
      for (j = 0; j < nsigs; j++)
        if (sigismember (&caught_signals, sig[j]))
          signal (sig[j], SIG_DFL);
#else
      for (j = 0; j < nsigs; j++)
        if (caught_sig[j])
          signal (sig[j], SIG_DFL);
#endif

      /* Act on any signals that arrived before the default was restored.
         This can process signals out of order, but there doesn't seem to
         be an easy way to do them in order, and the order isn't that
         important anyway.  */
      for (j = stop_signal_count; j; j--)
        raise (SIGSTOP);
      j = interrupt_signal;
      if (j)
        raise (j);
    }

  if (dired)
    {
      /* No need to free these since we're about to exit.  */
      dired_dump_obstack ("//DIRED//", &dired_obstack);
      dired_dump_obstack ("//SUBDIRED//", &subdired_obstack);
      printf ("//DIRED-OPTIONS// --quoting-style=%s\n",
              quoting_style_args[get_quoting_style (filename_quoting_options)]);
    }

  if (LOOP_DETECT)
    {
      assert (hash_get_n_entries (active_dir_set) == 0);
      hash_free (active_dir_set);
    }

  exit (exit_status);
}
