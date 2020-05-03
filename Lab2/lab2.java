class SyncObject 
{
  boolean ready = false;
  String msg;
}

class Сonsumer extends Thread
{
  SyncObject sync;

  public Сonsumer(SyncObject obj) {
    sync = obj;
  }

	@Override
	public void run()	{
    synchronized(sync) {
      while(sync.msg != "exit") {
        while(!sync.ready) {
          try {
              sync.wait();
          } catch (InterruptedException e) {
            System.out.println("Got InterruptedException, abort proccess...");
          }
        }

        System.out.println("Got message: " + sync.msg);
        sync.ready = false;
      }
    }
	}
}

class Provider 
{
  SyncObject sync;

  public Provider(SyncObject obj) {
    sync = obj;
  }

  public synchronized void provide() {
    synchronized(sync) {
      System.out.println("Send message: " + sync.msg);
      sync.ready = true;
      sync.notify();
    }
  }
}

class Main 
{
  public static void main(String[] args) {
    SyncObject sync = new SyncObject();

    Provider prod = new Provider(sync);
    Сonsumer con = new Сonsumer(sync);

    con.start();

    for(int i = 0; i < 5; i++) {
      sync.msg = Integer.toString(i);
      prod.provide();

      try {
        Thread.sleep(1000);
      } catch(InterruptedException e) {
        System.out.println("Got InterruptedException, abort proccess...");
      }
    }

    sync.msg = "exit";
    prod.provide();
  }
}