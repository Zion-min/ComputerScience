//�Էµ� NFA ǥ���Ŀ� ���� �Է¹��� String�� Accept�ϴ����� �Ǵ��ϴ� ���α׷�

import java.util.*;

class NFA
{   
	
  // M = (Q, ��, ��, q0, F)
  private int						Q;	    // internal states q ����
  private List<Character>	AlphaList;	 	// �� : input Alphabet���� ���� . string���� �����Ͽ� �־��ش�. 
  private int[][]			 tranfunc;      // ��[curstate][symbol] = nextstate : transition function (ex/ ��(q1,a)=q2) 
  private List<Integer>				F;      // F : final states ����
  
  
  public NFA(int Q)
  {
	  this.Q = Q;
	  this.AlphaList= new ArrayList<Character>();
	  this.tranfunc = new int[Q][128]; // ������ state�� �ڽ��� �� �ϳ����� ���´�. ASCII�ڵ� �� ���� 0~127�� column ����.
	  for(int i = 0;i <Q;i++)
		 for(int j = 0;j<128;j++)
			 tranfunc[i][j]=-1;
	  this.F= new ArrayList<Integer>(); 
	  
  }
  
  public int getQ() {
	return this.Q;  
  }
  
  public List<Character> getAlpha() {
	  return this.AlphaList;
  }
  
  public List<Integer> getF(){
	  return this.getF();
  }
  
  public void addFunc( int curr, char c, int next )
  {
   if ( (0<=curr && curr<Q) &&  (0<=next && next<Q) && ((32<=c && c<=127)) && (AlphaList.contains(c)) )
    	tranfunc[curr][c] = next;
   else if(!AlphaList.contains(c))
    	System.out.println("���� �߻�! "+c+"�� ������ �ȿ� �������� �ʽ��ϴ�.");
   else
    	System.out.println("���� �߻�! �� ���丶Ÿ�� ���� �����Լ��� ���� �� �����ϴ� : "+"��("+curr+","+c+")="+next);
  }
  
  public void addAlpha(char c)
  {
	  AlphaList.add(c);
  }
  
  public void addFinal( int finalstate )
  {
    F.add(finalstate);
  } 

  public boolean accept(String string)
  {
    int    curstate=0;		
    int    index=0;     
    char   cursymbol;  
   
    while ( index < string.length() && curstate != -1 )
    {
      cursymbol = string.charAt( index++ ) ;
      curstate  = tranfunc[curstate][cursymbol] ;
    }
  
    if ( index == string.length() && F.contains(curstate) )
      return true; 
    else
      return false;
  } 
  
  public void print() {
	  
	  System.out.println("M = (Q, ��, ��, q0, F)");
	  
	  System.out.print("Q = {");
	  for(int i = 0;i<Q;i++)
	  {
		  if(i==(Q-1))
		  {
			  System.out.printf("q%d}\n",i);
			  break;
		  }
		  System.out.printf("q%d, ",i);
	  }
	  
	  System.out.print("�� = {");
	  for (int i=0; i < AlphaList.size(); i++) 
	  {
		  if(i==(AlphaList.size()-1))
		  {
			  System.out.printf("%c}\n",AlphaList.get(i));
			  break;
		  }
		  System.out.printf("%c, ",AlphaList.get(i));
	  }
			
	  for(int i = 0;i <Q;i++)
		  for(int j = 0;j<128;j++)
			  if(tranfunc[i][j]!=-1)
			  {
				 System.out.println("��(q"+i+","+(char)j+") = q"+tranfunc[i][j]);
			  }
		
	  System.out.print("F = {");
	  for (int i=0; i < F.size(); i++) 
	  {
		  if(i==(F.size()-1))
		  {
			  System.out.printf("%d}\n",F.get(i));
			  break;
		  }
		  System.out.printf("%d, ",F.get(i));
	  }
	  
  }
  
  
}


public class DFAtester
{
  public static void main (String[] args)
  {
	int q,curr,next,f = 0;
	String str;
	char c = 0;
	
	Scanner scan = new Scanner(System.in);
	
	System.out.println("<<< ��������� DFA�� �־��� String�� Accept�ϴ��� �Ǵ��ϴ� ���α׷� >>>");
	System.out.println();
	System.out.print("initial state ���� > ");
    q = scan.nextInt();
	
    NFA fa = new NFA(q);
    
    scan.nextLine();
    System.out.println();
    System.out.println("Input Symbol �Է�(�Է��� �������� ' '�Է�) >>> ");
    while(!((str = scan.nextLine()).equals("")))
    {
    	c=str.charAt(0);
    	fa.addAlpha(c);
    }
    
    System.out.println();
    System.out.println("Transition Function �Է� >>> ");
    for(int i = 0;i <fa.getQ();i++)
    	for(int j = 0;j<fa.getAlpha().size();j++)
  		  {
    		System.out.println("��(q"+i+","+fa.getAlpha().get(j)+") = q"+(next = scan.nextInt()));
    		fa.addFunc(i, fa.getAlpha().get(j), next);
  		  }
    
    
    System.out.println();
    System.out.println("Final state �Է�(�Է��� �������� -1�Է�) >>> ");
    while(true)
    {
    	f = scan.nextInt();
    	if(f==-1)
    		break;
    	fa.addFinal(f);
    }
    
    System.out.println();
    System.out.println("����� ���� DFA�� ����մϴ� ...");  
    fa.print();
    
    
    System.out.println();
    System.out.print("Put a string >>> ");  

    scan.nextLine();
    while ( !((str = scan.nextLine()).equals("")) )
    { 
      if ( fa.accept( str ) )
        System.out.println("accept ����!");
      else
        System.out.println("accept �Ұ���!");
    
      System.out.print("Put a string >>>");  
    }
    System.out.println("�����մϴ�.");
  }

}
