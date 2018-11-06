object HelloApp {
  def main(args: Array[String]): Unit = {
    println("Hello " + args(0))
  }
}

def sum(a: Int, b: Int): Int = { return a + b }

def sum(a: Int, b: Int): Int = { a + b }

def sum(a: Int, b: Int): Int = a + b

def sum(a: Int, b: Int) = a + b

def printText(text: String):Unit = println(text)

def printText(text: String) = println(text)
