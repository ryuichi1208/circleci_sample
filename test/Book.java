package dokojava.book;

public class Book  extends TangibleAsset implements Thing{
	private String isbn;
	
	//コンストラクタ
	public Book(String name,int price,String color,double weight,String isbn) {
		super(name,price,color,weight);
		this.isbn = isbn;
	}
	
	//getterメソッド
	public String getIsbn() { return this.isbn; }
	
	
}