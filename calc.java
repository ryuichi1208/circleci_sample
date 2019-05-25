import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.Scanner;

/*
 * B - あまり α　の問題
 * https://atcoder.jp/contests/chokudai_S002/tasks/chokudai_S002_b
 *
 */

public class calc {

	public static void main(String[] args) {
		int zero = 0;
		String[] hint = { "が残りの回数です", "まだまだありますので頑張ってください" };

		System.out.println("キーボードから入力してください");
		// 計算回数を入力
		Scanner scan = new Scanner(System.in);
		int num = scan.nextInt();

		System.out.println("出力回数は「　" + num + "　」です");
		// 入力した回数チェック
		if (num != zero) {
			for (int i = 0; i < num; i++) {
				int cnt = 0;

				Scanner scan2 = new Scanner(System.in);
				// A1の数値を入力
				String num2 = scan2.next();
				System.out.println(num2);
				System.out.print(" ");
				// B1の数値を入力
				String num3 = scan2.next();
				System.out.println(num3);

				BigDecimal bigNum2 = new BigDecimal(num2);
				BigDecimal bigNum3 = new BigDecimal(num3);
				BigDecimal ans = new BigDecimal(0);

				if ((getVal(bigNum2)).compareTo(getVal(bigNum3)) >= 0) {
					ans = bigNum2.remainder(bigNum3);
// 						ans = bigNum2 .divide(bigNum3 , 0 , RoundingMode.FLOOR);
					// ans.setScale(0, RoundingMode.FLOOR);
				} else if (bigNum2.compareTo(bigNum3) < 0) {
					ans = bigNum2;
				}
				System.out.println(" 出力結果は　" + ans);
				//声援
				switch (i) {
				case 5:
					System.out.println(cnt +hint[0]);
					break;
				case 7:
					System.out.println(hint[1]);
					break;

				default:
					break;
				}
				cnt++;
			}

			System.out.println("出力回数は「　" + num + "　」です");

		}
	}

	/*
	 * nullのチェックメソッド
	 *
	 * <pre> BigDecimalの値がnullかの確認 <pre>
	 *
	 */
	public static BigDecimal getVal(BigDecimal val) {
		if (val != null) {
			return val;
		}
		return new BigDecimal(0);
	}

}
