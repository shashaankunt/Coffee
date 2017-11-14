class Clause {
  Clause(final int len, final int[] hgs, final int base, final int neck, final int[] xs) {
    this.hgs = hgs; // head+goals pointing to cells in cs
    this.base = base; // heap where this starts
    this.len = len; // length of heap slice
    this.neck = neck; // first after the end of the head
    this.xs = xs; // indexables in head
    
    
    System.out.println("Length:"+len);
    System.out.println("Base:"+base);
    System.out.println("Index XS:");
    for(int i=0; i<xs.length;i++)
    System.out.print("  "+xs[i]);
    System.out.println();
    System.out.println("Value of the neck:"+neck);
    System.out.println("Heap: ");
    for(int i=0; i<hgs.length;i++)
        System.out.print("  "+hgs[i]);
  }
  final int len; 
  final int[] hgs;
  final int base;
  final int neck;
  final int[] xs;
 
  }
public class ClauseTest{
	public static void main(String args[])
	{
		int [] ab={1,2,4};
		int [] bc={6,7,9};
		Clause cl=new Clause(10,ab, 7,3 ,bc);
		
	}

}
