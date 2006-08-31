package objectBrowser.connections;

public interface ITreeListener {
	public void add(TreeObject object);
	public void remove(TreeObject object);
	public void change(TreeObject object);
}
