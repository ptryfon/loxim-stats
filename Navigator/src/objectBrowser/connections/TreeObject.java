package objectBrowser.connections;



import org.eclipse.core.runtime.IAdaptable;

public interface TreeObject extends IAdaptable {
		public void addListener(ITreeListener listener);
		public void removeListener(ITreeListener listener);
		public void setParent(TreeParent parent);
		public TreeParent getParent();
	
}
