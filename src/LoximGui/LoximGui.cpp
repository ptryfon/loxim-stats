#include <QtGui> 
#include <iostream>
#include <sstream>
#include <LoximGui/LoximGui.h>
#include <LoximGui/ConnectDialog.h>
#include <LoximGui/Misc.h>
#include <Util/smartptr.h>
#include <LoximGui/Application.h>
#include <Client/Client.h>
#include <QtGui/QCheckBox>
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

static const string DEREF_STUDENTS("deref(student);");
static const string DEREF_TEACHERS("deref(teacher);");
static const string DEREF_ADMINISTRATORS("deref(administrator);");
static const string DEREF_PEOPLE("deref(person);");
static const string BEGIN("begin;");
static const string END("end;");

namespace LoximGui {

	LoximGuiWindow::LoximGuiWindow(QWidget *parent) : auth(this)
	{
		setupUi(this);
		connect(action_close, SIGNAL(triggered()), this, SLOT(exit_clicked()));
		connect(action_about, SIGNAL(triggered()), this, SLOT(show_about()));
		connect(action_connect, SIGNAL(triggered()), this, SLOT(connect_to_server()));
		connect(submit_sbql_button, SIGNAL(clicked()), this, SLOT(sbql_submit()));
		connect(clear_sbql, SIGNAL(clicked()), this, SLOT(clear_command_edit()));
		//connect(people_view, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(test_par(QTableWidgetItem *)));
		tab_widget->setDisabled(true);
		SimpleCallback::TCallback ttest_callback(_smptr::bind(&LoximGuiWindow::ttest, this));
		SimpleCallback::TCallback refresh_university_tab_callback(_smptr::bind(&LoximGuiWindow::refresh_university_tab, this));
		SimpleCallback::TCallback fill_people_callback(_smptr::bind(&LoximGuiWindow::fill_people, this));
		SimpleCallback::TCallback init_people_table_callback(_smptr::bind(&LoximGuiWindow::init_people_table, this));

		Application::get_instance().simpleCallback.add_callback(ttest_callback);
		Application::get_instance().simpleCallback.add_callback(fill_people_callback);
		Application::get_instance().simpleCallback.add_callback(refresh_university_tab_callback);
		Application::get_instance().simpleCallback.add_callback(init_people_table_callback);

		result_view->setColumnCount(1);
	}

	void LoximGuiWindow::test_par(QTableWidgetItem* item) {
		cout << "ITEM CHANGED: " << item->row() << " " << item->column() << endl;
		switch (item->column()) {
		case 1: {
			if (item->checkState() == Qt::Checked) {
				//del stud
				people_view->item(item->row(), 0)->setText(QString(""));
				stringstream ss;
				ss << "person where name = " << people_view->item(item->row(), 0)->text().toStdString() << ").destructor()";
				executeHelper(ss.str().c_str());

			} else {
				//add stud
				stringstream ss;
				if (people_view->item(item->row(), 2)->text().isNull() || people_view->item(item->row(), 2)->text().isEmpty()) {
					//empty cons

					ss << "Student includes (person where name = " << people_view->item(item->row(), 0)->text().toStdString() << ")).constructor();";

				} else {
					//param cons
					ss << "Student includes (person where name = " << people_view->item(item->row(), 0)->text().toStdString() << ")).constructor("<< people_view->item(item->row(), 2)->text().toStdString() <<");";
				}
				executeHelper(ss.str().c_str());
			}
		}
		default : {
			throw Errors::LoximException(EINVAL);
			}
		}
	}

	void LoximGuiWindow::executeHelper(string statement) {
		Application::get_instance().execute_stmt(BEGIN);
		Application::get_instance().execute_stmt(statement);
		Application::get_instance().execute_stmt(END);
	}

	void LoximGuiWindow::init_people_table() {
		QStringList qlt;
		people_view->setColumnCount(9);
		qlt.push_back(QString("name"));
		qlt.push_back(QString("is student"));
		qlt.push_back(QString("scholarship"));
		qlt.push_back(QString("is teacher"));
		qlt.push_back(QString("salary"));
		qlt.push_back(QString("is administrator"));
		qlt.push_back(QString("rate"));
		qlt.push_back(QString("hours worked"));
		qlt.push_back(QString("income"));
		people_view->setHorizontalHeaderLabels(qlt);
		fill_people_table();
	}

	void LoximGuiWindow::exit_clicked()
	{
		close();
	}

	void LoximGuiWindow::refresh_university_tab() {
		university_views_sbql_submit(students_view, DEREF_STUDENTS);
		university_views_sbql_submit(teachers_view,DEREF_TEACHERS);
		university_views_sbql_submit(administrator_view, DEREF_ADMINISTRATORS);
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
		sbql_submit(result_view, command_edit->toPlainText().toStdString());
		//cout << "person_container size:" << person_container.size() << endl ;
		cerr << "person_container size:" << person_container.size() << endl ;
		//cout << "person_containter 0 name " << person_container.get_string_at(0,"name") << endl;
	}

	void LoximGuiWindow::sbql_submit(QTreeWidget *result_view, const string &stmt) {
		result_view->clear();
		std::auto_ptr<Protocol::Package> result = Application::get_instance().execute_stmt(stmt);
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
				print_result(dynamic_cast<VSCSendvaluePackage&>(*result).get_val_data(), (QTreeWidgetItem*)0, result_view);
		}
	}

	void LoximGuiWindow::fill_people()
	{
		Application::get_instance().execute_stmt(BEGIN);
		std::auto_ptr<Protocol::Package> result = Application::get_instance().execute_stmt(DEREF_PEOPLE);
				if (!result.get())
					cout << "ABORTED" << endl;
					//result_view->addTopLevelItem(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Aborted"))));
				else {
					if (result->get_type() == A_SC_ERROR_PACKAGE) {
						stringstream ss;
						ASCErrorPackage &errHelp(dynamic_cast<ASCErrorPackage&>(*result));
						ss << "Got error: " << errHelp.get_val_error_code() << ":" << errHelp.get_val_description().to_string();
						cout << ss.str();
						//result_view->addTopLevelItem(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(ss.str().c_str()))));
					}
					if (result->get_type() == V_SC_SENDVALUE_PACKAGE)
						person_container.fill_person_containter(dynamic_cast<VSCSendvaluePackage&>(*result).get_val_data(), 0, 0);

				}
		Application::get_instance().execute_stmt(END);

	}

	void LoximGuiWindow::university_views_sbql_submit(QTreeWidget *result_view, const string &stmt) {
		Application::get_instance().execute_stmt(BEGIN);
		sbql_submit(result_view, stmt);
		Application::get_instance().execute_stmt(END);

	}

	void LoximGuiWindow::clear_command_edit()
	{
		command_edit->clear();
	}

	void LoximGuiWindow::print_result(const Package &package, QTreeWidgetItem *parent, QTreeWidget *result_view)
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
						print_result(**i, current, result_view);
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
						print_result(**i, current, result_view);
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
						print_result(**i, current, result_view);
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
					print_result(bp.get_val_value(), current, result_view);
				}
				break;
			default:
				stringstream ss;
				ss << "Unknown data type";
				result_view->addTopLevelItem(new QTreeWidgetItem(parent, QStringList(QString(ss.str().c_str()))));
		}
	}

	void LoximGuiWindow::fill_people_table() {
		int people_count = person_container.size();
		people_view->setRowCount(people_count);
		for (int i = 0; i < people_count; i++) {
			QTableWidgetItem *qtwi = new QTableWidgetItem(QString(person_container.get_string_at(i,"name").c_str()));
			people_view->setItem(i, 0, qtwi);
			QTableWidgetItem *qtwi2 = new QTableWidgetItem();
			qtwi2->data(Qt::CheckStateRole);
			if (person_container.has_attribute(i, "scholarship")) {
				qtwi2->setCheckState(Qt::Checked);

				people_view->setItem(i, 2, new QTableWidgetItem(QString(person_container.get_string_at(i,"scholarship").c_str())));
			}
			people_view->setItem(i, 1, qtwi2);

			QTableWidgetItem *qchbt = new QTableWidgetItem();
			if (person_container.has_attribute(i, "salary")) {
				qchbt->setCheckState(Qt::Checked);
				people_view->setItem(i, 4, new QTableWidgetItem(QString(person_container.get_string_at(i,"salary").c_str())));
			}
			people_view->setItem(i, 3, qchbt);

			QTableWidgetItem *qchba = new QTableWidgetItem();
			if (person_container.has_attribute(i, "rate") && person_container.has_attribute(i, "hoursWorked")) {
				qchba->setCheckState(Qt::Checked);
				people_view->setItem(i, 6, new QTableWidgetItem(QString(person_container.get_string_at(i,"rate").c_str())));
				people_view->setItem(i, 7, new QTableWidgetItem(QString(person_container.get_string_at(i,"hoursWorked").c_str())));
			}
			people_view->setItem(i, 5, qchba);

		}
	}

}
