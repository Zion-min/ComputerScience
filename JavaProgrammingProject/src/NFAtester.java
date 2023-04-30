//입력된 NFA 표현식에 따라 입력받은 String을 Accept하는지를 판단하는 프로그램

import java.util.*;

class NFA
{   
	
  // M = (Q, Σ, δ, q0, F)
  private int						Q;	    // internal states q 갯수
  private List<Character>	AlphaList;	 	// Σ : input Alphabet들의 집합 . string으로 연결하여 넣어준다. 
  private int[][]			 tranfunc;      // δ[curstate][symbol] = nextstate : transition function (ex/ δ(q1,a)=q2) 
  private List<Integer>				F;      // F : final states 집합
  
  
  public NFA(int Q)
  {
	  this.Q = Q;
	  this.AlphaList= new ArrayList<Character>();
	  this.tranfunc = new int[Q][128]; // 각각의 state가 자신의 행 하나씩을 갖는다. ASCII코드 를 넣을 0~127의 column 생성.
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
    	System.out.println("에러 발생! "+c+"이 Σ집합 안에 존재하지 않습니다.");
   else
    	System.out.println("에러 발생! 이 오토마타는 다음 전이함수를 읽을 수 없습니다 : "+"δ("+curr+","+c+")="+next);
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
	  
	  System.out.println("M = (Q, Σ, δ, q0, F)");
	  
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
	  
	  System.out.print("Σ = {");
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
				 System.out.println("δ(q"+i+","+(char)j+") = q"+tranfunc[i][j]);
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
	
	System.out.println("<<< 사용자정의 DFA가 주어진 String을 Accept하는지 판단하는 프로그램 >>>");
	System.out.println();
	System.out.print("initial state 갯수 > ");
    q = scan.nextInt();
	
    NFA fa = new NFA(q);
    
    scan.nextLine();
    System.out.println();
    System.out.println("Input Symbol 입력(입력을 끝내려면 ' '입력) >>> ");
    while(!((str = scan.nextLine()).equals("")))
    {
    	c=str.charAt(0);
    	fa.addAlpha(c);
    }
    
    System.out.println();
    System.out.println("Transition Function 입력 >>> ");
    for(int i = 0;i <fa.getQ();i++)
    	for(int j = 0;j<fa.getAlpha().size();j++)
  		  {
    		System.out.println("δ(q"+i+","+fa.getAlpha().get(j)+") = q"+(next = scan.nextInt()));
    		fa.addFunc(i, fa.getAlpha().get(j), next);
  		  }
    
    
    System.out.println();
    System.out.println("Final state 입력(입력을 끝내려면 -1입력) >>> ");
    while(true)
    {
    	f = scan.nextInt();
    	if(f==-1)
    		break;
    	fa.addFinal(f);
    }
    
    System.out.println();
    System.out.println("사용자 정의 DFA를 출력합니다 ...");  
    fa.print();
    
    
    System.out.println();
    System.out.print("Put a string >>> ");  

    scan.nextLine();
    while ( !((str = scan.nextLine()).equals("")) )
    { 
      if ( fa.accept( str ) )
        System.out.println("accept 가능!");
      else
        System.out.println("accept 불가능!");
    
      System.out.print("Put a string >>>");  
    }
    System.out.println("종료합니다.");
  }

}
