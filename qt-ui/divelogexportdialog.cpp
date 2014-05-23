#include <QFileDialog>
#include <QString>
#include <QShortcut>
#include <QAbstractButton>
#include <QDebug>
#include <QButtonGroup>

#include "mainwindow.h"
#include "divelogexportdialog.h"
#include "ui_divelogexportdialog.h"
#include "subsurfacewebservices.h"
#include "worldmap-save.h"

DiveLogExportDialog::DiveLogExportDialog(QWidget *parent) : QDialog(parent),
	ui(new Ui::DiveLogExportDialog)
{
	ui->setupUi(this);
	QShortcut *quit = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this);
	connect(quit, SIGNAL(activated()), parent, SLOT(close()));
	QShortcut *close = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this);
	connect(close, SIGNAL(activated()), this, SLOT(close()));
}

DiveLogExportDialog::~DiveLogExportDialog()
{
	delete ui;
}

void DiveLogExportDialog::on_exportGroup_buttonClicked(QAbstractButton *button)
{
	if (ui->exportUDDF->isChecked()) {
		ui->description->setText("UDDF is a generic format that enables communication among many dive computers and computer programs");
	} else if (ui->exportCSV->isChecked()) {
		ui->description->setText("CSV format, that includes the most critical information of the dive profile.");
	} else if (ui->exportDivelogs->isChecked()) {
		ui->description->setText("Subsurface XML format. This is the native format used by Subsurface.");
	} else if (ui->exportWorldMap->isChecked()) {
		ui->description->setText("Export the currently open dive logbook locations in HTML format and draw these on a world map.");
	}
}

void DiveLogExportDialog::on_buttonBox_accepted()
{
	QFileInfo fi(system_default_filename());
	QString filename;
	QString stylesheet;

	if (ui->exportUDDF->isChecked()) {
		stylesheet = "uddf-export.xslt";
		filename = QFileDialog::getSaveFileName(this, tr("Export UDDF File as"), fi.absolutePath(),
							tr("UDDF files (*.uddf *.UDDF)"));
	} else if (ui->exportCSV->isChecked()) {
		stylesheet = "xml2csv.xslt";
		filename = QFileDialog::getSaveFileName(this, tr("Export CSV File as"), fi.absolutePath(),
							tr("CSV files (*.csv *.CSV)"));
	} else if (ui->exportDivelogs->isChecked()) {
		DivelogsDeWebServices::instance()->prepareDivesForUpload(ui->exportSelected->isChecked());
		return;
	} else if (ui->exportWorldMap->isChecked()) {
		filename = QFileDialog::getSaveFileName(this, tr("Export World Map"), fi.absolutePath(),
							tr("HTML files (*.html)"));
		if (!filename.isNull() && !filename.isEmpty())
			export_worldmap_HTML(filename.toUtf8().data(), ui->exportSelected->isChecked());
		return;
	}

	if (!filename.isNull() && !filename.isEmpty())
		export_dives_xslt(filename.toUtf8(), ui->exportSelected->isChecked(), stylesheet.toStdString().c_str());
}
