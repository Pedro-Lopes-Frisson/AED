package java_solution;

public class Programador {
  private int busy_until;
  public Programador(){
    this.busy_until = -1;
  }
  public int getBusy_until(){
    return this.busy_until;
  }
  public void setBusy_until(int b){
    this.busy_until = b;
  }


}
