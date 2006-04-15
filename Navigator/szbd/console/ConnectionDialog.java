package szbd.console;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.BorderFactory;
import javax.swing.JButton;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.border.BevelBorder;
import javax.swing.border.LineBorder;
import javax.swing.border.SoftBevelBorder;
import javax.swing.border.TitledBorder;

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
public class ConnectionDialog extends javax.swing.JDialog {

	{
		//Set Look & Feel
		try {
			javax.swing.UIManager.setLookAndFeel(javax.swing.UIManager.getCrossPlatformLookAndFeelClassName());
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	private JLabel labelAddress;
	private JPanel panelButtons;
	private JPanel panelFields;
	private JButton buttonExit;
	private JTextField textServer;
	private JButton buttonOk;
	private JTextField textPort;
	private JLabel labelPort;
	private boolean modalResult = false;

	/**
	* Auto-generated main method to display this JDialog
	*/
	public static boolean showModal(ConnectionOptions opt) {
		JFrame frame = new JFrame();
		ConnectionDialog inst = new ConnectionDialog(frame);
		inst.setLocationRelativeTo(null);
		
		inst.setVisible(true);
				
		opt.host = inst.getServer();
		opt.port = new Integer(inst.getPort());
		return inst.modalResult;
	}
	
	public ConnectionDialog(JFrame frame) {
		super(frame);
		initGUI();
	}
	
	public String getServer() {
		return textServer.getText();
	}
	
	public String getPort() {
		return textPort.getText();
	}
	
	private void initGUI() {
		try {
			setModal(true);
			BorderLayout thisLayout = new BorderLayout();
			getContentPane().setLayout(thisLayout);
			this.setResizable(false);
			{
				panelButtons = new JPanel();
				FlowLayout panelButtonsLayout = new FlowLayout();
				panelButtonsLayout.setAlignment(FlowLayout.LEFT);
				panelButtons.setLayout(panelButtonsLayout);
				getContentPane().add(panelButtons, BorderLayout.SOUTH);
				{
					buttonOk = new JButton();
					panelButtons.add(buttonOk);
					buttonOk.setText("Connect");
					buttonOk.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent evt) {
							buttonOkActionPerformed(evt);
						}
					});
				}
				{
					buttonExit = new JButton();
					panelButtons.add(buttonExit);
					buttonExit.setText("Exit");
					buttonExit.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent evt) {
							buttonExitActionPerformed(evt);
						}
					});
				}
			}
			{
				panelFields = new JPanel();
				getContentPane().add(panelFields, BorderLayout.CENTER);
				GridBagLayout panelFieldsLayout = new GridBagLayout();
				panelFieldsLayout.rowWeights = new double[] {0.1, 0.1};
				panelFieldsLayout.rowHeights = new int[] {7, 7};
				panelFieldsLayout.columnWeights = new double[] {0.1, 0.1};
				panelFieldsLayout.columnWidths = new int[] {7, 7};
				panelFields.setLayout(panelFieldsLayout);
				panelFields.setBorder(BorderFactory.createCompoundBorder(
					BorderFactory.createEmptyBorder(3, 3, 3, 3),
					BorderFactory.createEtchedBorder(BevelBorder.LOWERED)));
				{
					labelAddress = new JLabel();
					panelFields.add(labelAddress, new GridBagConstraints(0, 0, 2, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
					labelAddress.setText("Server address:");
					labelAddress.setPreferredSize(new java.awt.Dimension(100, 17));
					labelAddress.setBorder(BorderFactory.createEmptyBorder(
						3,
						3,
						3,
						3));
				}
				{
					textServer = new JTextField();
					panelFields.add(textServer, new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
					textServer.setText("192.168.0.184");
					textServer.setPreferredSize(new java.awt.Dimension(140, 20));
					textServer.setSize(140, 20);
				}
				{
					textPort = new JTextField();
					panelFields.add(textPort, new GridBagConstraints(1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
					textPort.setText("6543");
					textPort.setPreferredSize(new java.awt.Dimension(40, 19));
					textPort.setSize(32, -1);
				}
				{
					labelPort = new JLabel();
					panelFields.add(labelPort, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
					labelPort.setText("Port:");
					labelPort.setBorder(BorderFactory.createEmptyBorder(
						3,
						3,
						3,
						3));
					labelPort.setPreferredSize(new java.awt.Dimension(100, 28));
				}
			}
			this.setSize(337, 155);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void buttonExitActionPerformed(ActionEvent evt) {
		modalResult = false;
		setVisible(false);
	}
	
	private void buttonOkActionPerformed(ActionEvent evt) {
		modalResult = true;
		setVisible(false);		
	}

}
