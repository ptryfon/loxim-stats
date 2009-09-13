#include <QtGui> 
#include <iostream>
#include <sstream>
#include <LoximGui/LoximGui.h>
#include <LoximGui/ConnectDialog.h>
#include <LoximGui/Misc.h>
#include <Util/smartptr.h>
#include <LoximGui/Application.h>
#include <Client/Client.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/Packages/ASCByePackage.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Protocol/Packages/ASCOkPackage.h>
#include <Protocol/Packages/ASCPingPackage.h>
#include <Protocol/Packages/ASCPongPackage.h>
#include <Protocol/Packages/ASCSetoptPackage.h>
#include <Protocol/Packages/QCExecutePackage.h>
#include <Protocol/Packages/QCStatementPackage.h>
#include <Protocol/Packages/VSCAbortPackage.h>
#include <Protocol/Packages/VSCFinishedPackage.h>
#include <Protocol/Packages/VSCSendvaluePackage.h>
#include <Protocol/Packages/VSCSendvaluesPackage.h>
#include <Protocol/Packages/WCHelloPackage.h>
#include <Protocol/Packages/WCLoginPackage.h>
#include <Protocol/Packages/WCPasswordPackage.h>
#include <Protocol/Packages/WSAuthorizedPackage.h>
#include <Protocol/Packages/WSHelloPackage.h>
#include <Protocol/Packages/Data/BagPackage.h>
#include <Protocol/Packages/Data/BindingPackage.h>
#include <Protocol/Packages/Data/BoolPackage.h>
#include <Protocol/Packages/Data/DoublePackage.h>
#include <Protocol/Packages/Data/RefPackage.h>
#include <Protocol/Packages/Data/SequencePackage.h>
#include <Protocol/Packages/Data/Sint32Package.h>
#include <Protocol/Packages/Data/StructPackage.h>
#include <Protocol/Packages/Data/VarcharPackage.h>
#include <Protocol/Packages/Data/VoidPackage.h>
#include <Util/smartptr.h>

using namespace Protocol;
using namespace std;
using namespace _smptr;

namespace LoximGui {

	LoximGuiWindow::LoximGuiWindow(QWidget *parent) : auth(this)
	{
		setupUi(this);
		connect(action_close, SIGNAL(triggered()), this, SLOT(exit_clicked()));
		connect(action_about, SIGNAL(triggered()), this, SLOT(show_about()));
		connect(action_connect, SIGNAL(triggered()), this, SLOT(connect_to_server()));
		connect(submit_sbql_button, SIGNAL(clicked()), this, SLOT(sbql_submit()));
		connect(clear_sbql, SIGNAL(clicked()), this, SLOT(clear_command_edit()));
		tab_widget->setDisabled(true);
		SimpleCallback::TCallback ttest_callback(_smptr::bind(&LoximGuiWindow::ttest, this));
		Application::get_instance().simpleCallback.add_callback(ttest_callback);
		result_view->setColumnCount(1);
	}

	void LoximGuiWindow::exit_clicked()
	{
		close();
	}

	void LoximGuiWindow::show_about()
	{
		QMessageBox::about(this,"About LoXiMGui",
			"LoXiMGui is a simple program for demonstrating LoXiM functionality."
			"\n\n License: LGPL-v3\n\n"
			"Authors: Marek Dopiera, Grzegorz Timoszuk");
	}

	void LoximGuiWindow::connect_to_server()
	{
		ConnectDialog *cd =  new ConnectDialog(this);
		cd->show();
	}

	void LoximGuiWindow::ttest() 
	{
		tab_widget->setEnabled(true);
		command_edit->clear();
	}

	void LoximGuiWindow::sbql_submit() 
	{
		result_view->clear();
		std::auto_ptr<Protocol::Package> result = Application::get_instance().execute_stmt(command_edit->toPlainText().toStdString());
		if (!result.get())
			result_view->addTopLevelItem(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Aborted"))));
		else {
			if (result->get_type() == A_SC_ERROR_PACKAGE) {
				stringstream ss;
				ASCErrorPackage &errHelp(dynamic_cast<ASCErrorPackage&>(*result));
				ss << "Got error: " << errHelp.get_val_error_code() << ":" << errHelp.get_val_description().to_string();
				result_view->addTopLevelItem(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(ss.str().c_str()))));
			}
			if (result->get_type() == V_SC_SENDVALUE_PACKAGE)
				print_result(dynamic_cast<VSCSendvaluePackage&>(*result).get_val_data(), (QTreeWidgetItem*)0);
		}
	}

	void LoximGuiWindow::clear_command_edit()
	{
		command_edit->clear();
	}

	void LoximGuiWindow::print_result(const Package &package, QTreeWidgetItem* parent)
	{
		switch (package.get_type()){
			case BAG_PACKAGE:
				{
					QTreeWidgetItem* current = new QTreeWidgetItem(parent, QStringList(QString("Bag")));
					result_view->addTopLevelItem(current);
					const BagPackage &bag(dynamic_cast<const
							BagPackage&>(package));
					const vector<shared_ptr<Package> >
						&items(bag.get_packages());
					for (vector<shared_ptr<Package> >::const_iterator
							i = items.begin();
							i != items.end();
							++i)
						print_result(**i, current);
				}
				break;
			case STRUCT_PACKAGE:
				{
					QTreeWidgetItem* current = new QTreeWidgetItem(parent, QStringList(QString("Struct")));
					result_view->addTopLevelItem(current);
					const StructPackage &strct(dynamic_cast<const
							StructPackage&>(package));
					const vector<shared_ptr<Package> >
						&items(strct.get_packages());
					for (vector<shared_ptr<Package> >::const_iterator
							i = items.begin();
							i != items.end();
							++i)
						print_result(**i, current);
				}
				break;
			case SEQUENCE_PACKAGE:
				{
					QTreeWidgetItem* current = new QTreeWidgetItem(parent, QStringList(QString("Sequence")));
					result_view->addTopLevelItem(current);
					const SequencePackage &seq(dynamic_cast<const
							SequencePackage&>(package));
					const vector<shared_ptr<Package> >
						&items(seq.get_packages());
					for (vector<shared_ptr<Package> >::const_iterator
							i = items.begin();
							i != items.end();
							++i)
						print_result(**i, current);
					break;
				}
			case REF_PACKAGE:
				{
					stringstream ss;
					ss  << "ref(" <<
						dynamic_cast<const RefPackage&>(package).get_val_value_id() << ")";
					result_view->addTopLevelItem(new QTreeWidgetItem(parent, QStringList(QString(ss.str().c_str()))));
					break;
				}
			case VARCHAR_PACKAGE:
				{
					stringstream ss;
					ss << dynamic_cast<const VarcharPackage&>(package).get_val_value().to_string();
					result_view->addTopLevelItem(new QTreeWidgetItem(parent,  QStringList(QString(ss.str().c_str()))));
					break;
				}
			case VOID_PACKAGE:
				{
					stringstream ss;
					ss << "void" ;
					result_view->addTopLevelItem(new QTreeWidgetItem(parent,  QStringList(QString(ss.str().c_str()))));
					break;
				}
			case SINT32_PACKAGE:
				{
					stringstream ss;
					ss << dynamic_cast<const Sint32Package&>(package).get_val_value();
					result_view->addTopLevelItem(new QTreeWidgetItem(parent,  QStringList(QString(ss.str().c_str()))));
					break;
				}
			case DOUBLE_PACKAGE:
				{
					stringstream ss;
					ss << dynamic_cast<const DoublePackage&>(package).get_val_value();
					result_view->addTopLevelItem(new QTreeWidgetItem(parent,  QStringList(QString(ss.str().c_str()))));
					break;
				}
			case BOOL_PACKAGE:
				{
					stringstream ss;
					ss << dynamic_cast<const	BoolPackage&>(package).get_val_value();
					result_view->addTopLevelItem(new QTreeWidgetItem(parent,  QStringList(QString(ss.str().c_str()))));
					break;
				}
			case BINDING_PACKAGE:
				{
					const BindingPackage &bp(dynamic_cast<const
							BindingPackage&>(package));
					stringstream ss;
					ss << bp.get_val_binding_name().to_string();
					QTreeWidgetItem* current = new QTreeWidgetItem(parent, QStringList(QString(ss.str().c_str())));
					result_view->addTopLevelItem(current);
					print_result(bp.get_val_value(), current);
				}
				break;
			default:
				stringstream ss;
				ss << "Unknown data type";
				result_view->addTopLevelItem(new QTreeWidgetItem(parent, QStringList(QString(ss.str().c_str()))));
		}
	}


}
