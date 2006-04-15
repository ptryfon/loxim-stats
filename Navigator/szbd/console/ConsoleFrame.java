package szbd.console;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;

import javax.swing.WindowConstants;

import szbd.client.Connection;
import szbd.client.result.Result;


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
public class ConsoleFrame extends javax.swing.JFrame {

	{
		//Set Look & Feel
		try {
			javax.swing.UIManager.setLookAndFeel(javax.swing.UIManager.getCrossPlatformLookAndFeelClassName());
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	private JTextArea textAreaQuery;
	private JPanel panelQuery;
	private JScrollPane scrollPaneQuery;
	private JSplitPane splitPane;
	private JButton buttonClear;
	private JButton buttonExecute;
	private JPanel panelButtons;
	private JScrollPane ScrollPaneTextResult;
	private JTextArea textAreaResult;
	private JTabbedPane PanelResult;
	
	private Connection con;

	/**
	* Auto-generated main method to display this JFrame
	*/
	
	public ConsoleFrame(Connection _con) {
		super();
		con = _con;
		initGUI();
	}
	
	private void initGUI() {
		try {
			BorderLayout thisLayout = new BorderLayout();
			getContentPane().setLayout(thisLayout);
			{
				splitPane = new JSplitPane();
				getContentPane().add(splitPane, BorderLayout.CENTER);
				splitPane.setOrientation(JSplitPane.VERTICAL_SPLIT);
				{
					panelQuery = new JPanel();
					splitPane.add(panelQuery, JSplitPane.TOP);
					BorderLayout panelQueryLayout = new BorderLayout();
					panelQuery.setLayout(panelQueryLayout);
					panelQuery.setPreferredSize(new java.awt.Dimension(400, 195));
					panelQuery.setFocusCycleRoot(true);
					{
						scrollPaneQuery = new JScrollPane();
						panelQuery.add(scrollPaneQuery, BorderLayout.CENTER);
						{
							textAreaQuery = new JTextArea();
							scrollPaneQuery.setViewportView(textAreaQuery);
							textAreaQuery
								.setPreferredSize(new java.awt.Dimension(
									400,
									184));
						}
					}
					{
						panelButtons = new JPanel();
						BoxLayout panelButtonsLayout = new BoxLayout(
							panelButtons,
							javax.swing.BoxLayout.X_AXIS);
						panelQuery.add(panelButtons, BorderLayout.SOUTH);
						panelButtons.setPreferredSize(new java.awt.Dimension(464, 24));
						panelButtons.setLayout(panelButtonsLayout);
						{
							buttonExecute = new JButton();
							panelButtons.add(buttonExecute);
							buttonExecute.setText("Execute");
							buttonExecute
								.addActionListener(new ActionListener() {
								public void actionPerformed(ActionEvent evt) {
									buttonExecuteActionPerformed(evt);
								}
								});
						}
						{
							buttonClear = new JButton();
							panelButtons.add(buttonClear);
							buttonClear.setText("Clear");
						}
					}
				}
				{
					PanelResult = new JTabbedPane();
					splitPane.add(PanelResult, JSplitPane.BOTTOM);
					PanelResult.setPreferredSize(new java.awt.Dimension(
						400,
						103));
					{
						ScrollPaneTextResult = new JScrollPane();
						PanelResult.addTab("Result (Text)", null, ScrollPaneTextResult, null);
						{
							textAreaResult = new JTextArea();
							ScrollPaneTextResult
								.setViewportView(textAreaResult);
						}
					}
				}
			}
			setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
			this.setTitle("SBQL Console");
			this.addWindowListener(new WindowAdapter() {
				public void windowClosed(WindowEvent evt) {
					rootWindowClosed(evt);
				}
			});
			pack();
			this.setSize(474, 371);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void buttonExecuteActionPerformed(ActionEvent evt) {
		try {
			Result res = con.execute(textAreaQuery.getText());
			textAreaResult.setText(res.toString());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void rootWindowClosed(WindowEvent evt) {
		try {
			con.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
