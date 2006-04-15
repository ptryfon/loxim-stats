package szbd.test;
import java.awt.BorderLayout;
import java.util.List;

import javax.swing.JScrollPane;
import javax.swing.JTree;

import javax.swing.WindowConstants;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.MutableTreeNode;

import szbd.metadata.ComplexTypeDefinition;
import szbd.metadata.TypeDefinition;


/**
* This code was edited or generated using CloudGarden's Jigloo
* SWT/Swing GUI Builder, which is free for non-commercial
* use. If Jigloo is being used commercially (ie, by a corporation,
* company or business for any purpose whatever) then you
* should purchase a license for each developer using Jigloo.
* Please visit www.cloudgarden.com for details.
* Use of Jigloo implies acceptance of these licensing terms.
* A COMMERCIAL LICENSE HAS NOT BEEN PURCHASED FOR
* THIS MACHINE, SO JIGLOO OR THIS CODE CANNOT BE USED
* LEGALLY FOR ANY CORPORATE OR COMMERCIAL PURPOSE.
*/
public class MetadataBrowser extends javax.swing.JFrame {
	private JScrollPane scrollPaneMetadata;
	private JTree treeMetadata;

	/**
	* Auto-generated main method to display this JFrame
	*/
	
	List<TypeDefinition> ld;
	
	public MutableTreeNode getRoot() {
		DefaultMutableTreeNode root = new DefaultMutableTreeNode("ROOT");
		for (TypeDefinition ct : ld) {
			root.add(getNode(ct));
		}		
		return root;
	}	
	
	public MutableTreeNode getNode(TypeDefinition t) {
		DefaultMutableTreeNode root = new DefaultMutableTreeNode(t.toString());
		if (t instanceof ComplexTypeDefinition) {
		for (TypeDefinition ct : ((ComplexTypeDefinition)t).getItems()) {
			root.add(getNode(ct));
		}
		}
		return root;
	} 
	
	public MetadataBrowser(List<TypeDefinition> _ld) {
		super();
		ld = _ld;
		initGUI();		
	}
	
	private void initGUI() {
		try {
			setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
			{
				scrollPaneMetadata = new JScrollPane();
				getContentPane().add(scrollPaneMetadata, BorderLayout.CENTER);
				{
					treeMetadata = new JTree(getRoot());					
					scrollPaneMetadata.setViewportView(treeMetadata);
				}
			}
			pack();
			setSize(400, 300);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
