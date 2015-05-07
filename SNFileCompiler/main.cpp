
#include <QtCore/QCoreApplication>
#include <stdint.h>
#include "../Common/SNStringEncryptor.h"
#include <QFile>
#include "QTextStream"
#include <QtGui/QLabel>

SNStringEncryptor se;

void GenerateArrayFile(const char* src_filename, const char* array_name)
{
	QFile f(src_filename);
	f.open(QIODevice::ReadOnly);
	QByteArray arr = f.readAll();

	f.close();

	se.Crypt(arr.data(), arr.size());
	
	QString array_string = QString("#ifndef %1_h \r\n#define %1_h\r\n char %1[] = {").arg(array_name);

	for (int32_t i = 0; i < arr.size(); ++i)
	{
		array_string += QString("0x%1").arg((uint8_t)arr[i], 2, 16, QChar('0'));

		if (i == arr.size() - 1)
			array_string += "};\r\n#endif";
		else
			array_string += ", ";
	}

	QFile out_file(QString("%1.h").arg(array_name));
	out_file.open(QIODevice::WriteOnly);
	QTextStream ts(&out_file);

	ts << array_string;

	out_file.close();
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	GenerateArrayFile("../ANNTrainer/best.ann", "ann_config");
	GenerateArrayFile("../cascade_2.xml", "plate_detector_cascade");

	return a.exec();
}
