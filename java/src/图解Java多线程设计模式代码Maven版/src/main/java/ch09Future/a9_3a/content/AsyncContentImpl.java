package ch09Future.a9_3a.content;

class AsyncContentImpl implements Content {
    private SyncContentImpl synccontent;
    private boolean ready = false;

    public synchronized void setContent(SyncContentImpl synccontent) {
        this.synccontent = synccontent;
        this.ready = true;
        notifyAll();
    }

    public synchronized byte[] getBytes() {
        while (!ready) {
            try {
                wait();
            } catch (InterruptedException e) {
            }
        }
        return synccontent.getBytes();
    }
}
