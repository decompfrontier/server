package it.arves100.gimuserver;

/**
 * Access class for the local server used in Brave Frontier offline mod
 */
public class OfflineMod {

    /**
     * Offline server thread
     */
    static class OfflineServerThread implements Runnable {

        /**
         * Runs the server
         */
        @Override
        public void run() {
            startup();
        }
    }

    /**
     * Starts up the web server in the executed thread
     * @note This is the entrypoint of the web server
     */
    public static native void startup();

    /**
     * Creates a new thread and executes the web server
     */
    public static void startOfflineServer()
    {
        try
        {
            System.loadLibrary("offlinemod");
            new Thread(new OfflineServerThread()).start();
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }
}
