#include "editagentdialog.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const int ID      = 0;
const int NAME    = 1;
const int ADDRESS = 2;
const int PHONE   = 3;

EditAgentDialog::EditAgentDialog(QDialog *parent) : QDialog( parent, Qt::Tool ) {
    setupUi(this);

    tableWidget->hideColumn(ID);
    populateTableWidget();

    connect( tableWidget, SIGNAL( itemClicked(QTableWidgetItem *) ), this, SLOT( fetchItem(QTableWidgetItem *) ) );
    connect( saveButton,  SIGNAL(clicked()),                         this, SLOT(save()) );
}

void EditAgentDialog::populateTableWidget() {
    QSqlQuery query;
    query.prepare("SELECT * FROM agent");
    if ( !query.exec() ) {
        qDebug() << query.lastError();
        qFatal("Failed to execute query.");
    }

    tableWidget->setRowCount(query.size());

    tableWidget->setColumnWidth( ID,      50);
    tableWidget->setColumnWidth( NAME,    200);
    tableWidget->setColumnWidth( ADDRESS, 200);
    tableWidget->setColumnWidth( PHONE,   150);

    int row = 0;

    while ( query.next() ) {
        QTableWidgetItem *idItem      = new QTableWidgetItem;
        QTableWidgetItem *nameItem    = new QTableWidgetItem;
        QTableWidgetItem *addressItem = new QTableWidgetItem;
        QTableWidgetItem *phoneItem   = new QTableWidgetItem;

        qDebug() << "EditAgentDialog::populateTableWidget() - " << "ID: "      << query.value(ID).toString();
        qDebug() << "EditAgentDialog::populateTableWidget() - " << "Name: "    << query.value(NAME).toString();
        qDebug() << "EditAgentDialog::populateTableWidget() - " << "Address: " << query.value(ADDRESS).toString();
        qDebug() << "EditAgentDialog::populateTableWidget() - " << "Phone: "   << query.value(PHONE).toString();

        idItem->setText( query.value(ID).toString() );
        nameItem->setText( query.value(NAME).toString() );
        addressItem->setText( query.value(ADDRESS).toString() );
        phoneItem->setText( query.value(PHONE).toString() );

        tableWidget->setItem( row,   ID,      idItem );
        tableWidget->setItem( row,   NAME,    nameItem );
        tableWidget->setItem( row,   ADDRESS, addressItem );
        tableWidget->setItem( row++, PHONE,   phoneItem );
    }
}

void EditAgentDialog::fetchItem(QTableWidgetItem *item) {
    currentRow = tableWidget->row(item);

    nameEdit->setText( tableWidget->item( currentRow,    NAME )->text() );
    addressEdit->setText( tableWidget->item( currentRow, ADDRESS )->text() );
    phoneEdit->setText( tableWidget->item( currentRow,   PHONE )->text() );
}

void EditAgentDialog::save() {
    if ( nameEdit->text() == "" || addressEdit->toPlainText() == "" || phoneEdit->text() == "" ) {
        QMessageBox *msgbox = new QMessageBox(
            QMessageBox::Information, "Incomplete Fields",
            "All fields are to be filled.", QMessageBox::Ok );
        msgbox->show();

        return;
    }

    qDebug() << "EditAgentDialog::save() - " << "Name: "    + nameEdit->text();
    qDebug() << "EditAgentDialog::save() - " << "Address: " + addressEdit->toPlainText();
    qDebug() << "EditAgentDialog::save() - " << "Phone: "   + phoneEdit->text();

    QSqlQuery query;
    query.prepare("UPDATE agent SET name = :name, address = :address, phone = :phone WHERE id = :id");

    query.bindValue( ":id",      tableWidget->item( currentRow, ID )->text() );
    query.bindValue( ":name",    nameEdit->text() );
    query.bindValue( ":address", addressEdit->toPlainText() );
    query.bindValue( ":phone",   phoneEdit->text() );

    if ( !query.exec() ) {
        qDebug() << query.lastError();
        qFatal("Failed to execute query.");
    }

    nameEdit->clear();
    addressEdit->clear();
    phoneEdit->clear();

    populateTableWidget();
    
    nameEdit->setFocus();
}
