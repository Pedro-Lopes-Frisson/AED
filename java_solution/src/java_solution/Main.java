package java_solution;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;
import java.util.Random;

public class Main {
	static int tail = 100;
	static int sum1 = 298;
	static int MAX_T = 64;
	static int MAX_P = 10;

	public static void main(String argv[]) {
	  Problem_T problem = new Problem_T();
	  for(int i = 0 ; i< MAX_T; i++)
	  {
	  	problem.task[i] = new Task();
	  	if( i < MAX_P) problem.prog[i] = new Programador();
	  }
	  Integer NMec,T,P,I;
	  int argc = argv.length;
		NMec = (argc < 2) ? 2020 : Integer.parseInt(argv[0]) ;
	  T = (argc < 3) ? 5 : Integer.parseInt(argv[1]);
	  P = (argc < 4) ? 2 : Integer.parseInt(argv[2]);
	  I = (argc < 5) ? 0 : Integer.parseInt(argv[3]);
	  init_problem(NMec,T,P,I,problem);
	  try {
			solve(problem);
		} catch (IOException e) {
			e.printStackTrace();
		}

	}
	public static void init_problem(Integer nmec, Integer T, Integer P, Integer I, Problem_T problem) {
		  int i,r,scale,span,total_span;
		  //
		  // input validation
		  //

		  if(nmec < 1 || nmec > 999999)
		  {
		  System.err.print(String.format("Bad NMec (1 <= NMex (%d) <= 999999)\n",nmec));
		  System.exit(-1);
		  }
		  if(T < 1 || T > MAX_T)
		  {
		  System.err.print(String.format("Bad T (1 <= T (%d) <= %d)\n",T,MAX_T));
			  System.exit(-1);
		  }
		  if(P < 1 || P > MAX_P)
		  {
		  	System.err.print(String.format("Bad P (1 <= P (%d) <= %d)\n",P,MAX_P));
		  System.exit(-1);
		  }
		  total_span = (10 * T + P - 1) / P;
		  if(total_span < 30)
        total_span = 30;
		  final double sum2 = total_span - 1;
		  int[] weight = new int[total_span + 1];
		  scale = (int) Math.ceil((double) tail * 10.0 * sum2 / sum1);
		  if(scale < tail) scale = tail;
		  weight[0] = 0;
		  weight[1] = 0;
		  for(i = 2;i <= 10;i++)
        weight[i] = scale * (2 * i);
		  for(i = 11;i <= 29;i++)
        weight[i] = scale * (30 - i);
		  for(i = 30;i <= total_span;i++)
        weight[i] = tail;
		  for(i = 1;i <= total_span;i++)
        weight[i] += weight[i - 1];
		  Random random = new Random();
		  random.setSeed(nmec + 314161 * T + 271829 * P);
		  problem.setNMec(nmec);
		  problem.setP(P);
		  problem.setT(T);
		  problem.setI(I);
		  for(i = 0;i < T;i++)
		  {
		  r = 1 + random.nextInt() % weight[total_span]; // 1 .. weight[total_span]
		  for(span = 0;span < total_span;span++)
		    if(r <= weight[span])
		    break;
		  problem.task[i].setStartingDate((int)random.nextInt() % (total_span - span + 1 ));
		  problem.task[i].setEndingDate(problem.task[i].getStarting_date() + span - 1);
		  //
		  // task profit
		  //
		  // the task profit is given by r*task_span, where r is a random variable in the range 50..300 with a probability
		  //   density function with shape (two triangles, the area of the second is 4 times the area of the first)
		  //
		  //    *
		  //   /|   *
		  //  / |     *
		  //   /  |       *
		  //  *---*---------------*
		  // 50 100 150 200 250 300
		  //
		  scale = (int)random.nextInt() % 12501; // almost uniformly distributed in 0..12500
		  if(scale <= 2500)
		    problem.task[i].setProfit( 1 + (int)Math.round((double)span * (50.0 + Math.sqrt((double)scale))));
		  else
		    problem.task[i].setProfit(1 + (int)Math.round((double)span * (300.0 - 2.0 * Math.sqrt((double)(12500 - scale)))));
		  }
		  Arrays.sort(problem.task);
	}
	public static void solve(Problem_T problem) throws IOException {
  File current_path =  new File( System.getProperty("user.dir"));
  File fp = null;
  FileWriter fpStream = null;
	try{
    fp = new File(current_path, String.format("%02d_%02d_%d.txt",problem.T,problem.P,problem.I));
    fpStream = new FileWriter(fp, false); // true to append
  }
		catch(Exception e)
  {
    System.err.println("Something went wrong while opening files");
    System.out.println(e.getMessage());
    System.exit(-1);
  }
  // solvve problem
  final long startTime = System.currentTimeMillis();

// place solution

  for(int t = 0; t < problem.T ; t++)
  {
    problem.task[t].setAssigned_to(-1);
    problem.task[t].setBest_assigned_to(-1);
  }

  for(int p = 0; p < problem.P ; p++)
  {
    problem.prog[p].setBusy_until(-1);
  

  problem.setCasos( 0);
  problem.setBiggestP(0);
  problem.setTotal_profit(0);
  
  Arrays.sort(problem.task);

  if ( problem.I == 0 )
  {
    System.out.println("recurse");
    recurse(problem, 0);
  }
  else
  {
    System.out.println("nrecurse");
    for(int g = 0; g < problem.P; g++)
      nonRec(problem, g);
  }
  //endind time
  final long endingTime = System.currentTimeMillis();
  problem.setCpu_time(endingTime-startTime);
  //
  // save solution data
  //
  for(int t = 0; t < problem.T; t++)
  {
    System.out.println(String.format("P%d\t%d T%d %d \n",problem.task[t].getBest_assigned_to(),problem.task[t].getStarting_date(),t,problem.task[t].getEnding_date()));
  }
  System.out.println(String.format("NMec = %d\n",problem.getNMec()));
  System.out.println(String.format("Viable Sol. = %d\n",problem.getCasos()));
  System.out.println(String.format("Profit = %d\n",problem.getBiggestP()));
  System.out.println(String.format("T = %d\n",problem.getT()));
  System.out.println(String.format("P = %d\n",problem.getP()));
  System.out.println(String.format("Profits%s ignored\n",(problem.getI() == 0) ? " not" : ""));
  System.out.println(String.format("Solution time = %d\n",problem.getCpu_time()));
  System.out.println(String.format("Task data\n"));

  for(int i = 0;i < problem.T;i++)
    System.out.println(String.format("%02d  %3d %3d %5d\n",i,problem.task[i].getStarting_date(),problem.task[i].getEnding_date(),problem.getTotal_profit()));

  System.out.println(String.format("End\n"));
  //
  // terminate
  //
  fpStream.close();
  }


	}
	private static int recurse(Problem_T prob, int t) {

	  int busy_save;
	  int profit_save;
	  int task_save;
	  if(t >= prob.getT())
	  {
	    prob.incCasos();
	    if(prob.getTotal_profit() > prob.getBiggestP())
	    {
	      prob.setBiggestP(prob.getTotal_profit());
	      for(int j = 0; j < prob.getT(); j++)
	      {
	        prob.task[j].setBest_assigned_to(prob.task[j].getAssigned_to());
	      }
	    }
	    return 1;
	  }

	  recurse(prob, t + 1);

	  for(int p = 0; p < prob.getP(); p++)
	  {
	    if(prob.prog[p].getBusy_until() < prob.task[t].getStarting_date())
	    {
	      if(prob.task[t].getAssigned_to() < 0)
	      {
	        busy_save = prob.prog[p].getBusy_until();
	        task_save = prob.task[t].getAssigned_to();
	        profit_save = prob.getTotal_profit();

	        prob.prog[p].setBusy_until(prob.task[t].getEnding_date());
	        prob.task[t].setAssigned_to(p);
	        prob.sumTotal_profit(prob.task[t].getProfit());
	        recurse(prob, t+1);
	        prob.prog[p].setBusy_until( busy_save );
	        prob.task[t].setAssigned_to(task_save );
	        prob.setTotal_profit(profit_save);
	      }
	    }
	  }
	  return 0;

		
	}
	private static int nonRec(Problem_T problem, int g) {
				  int t = 0;
				  for( t = 0; t < problem.getT(); t++){
				    if( problem.task[t].getStarting_date() > problem.prog[g].getBusy_until() && problem.task[t].getBest_assigned_to() == -1 )
				    {
				      problem.prog[g].setBusy_until(problem.task[t].getEnding_date());
				      problem.sumBiggest_profit(problem.task[t].getProfit());
				      problem.task[t].setBest_assigned_to(g);
				    }
				  }
				  return 1;
	
	}

}
