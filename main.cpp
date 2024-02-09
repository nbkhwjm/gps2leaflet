#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QtMath>

int main(int argc, char *argv[])
{
    double lat = 0.0;
    double lon = 0.0;
    int zoom = 0;
    double xtile = 0;
    double ytile = 0;
    bool debug = false;
    bool help = false;
    QString output = "";
    QString xtile_string = "";
    QString ytile_string = "";

    QTextStream out = QTextStream(stdout);

    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("gps2leaflet");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("GPS to Leaflet Coordinate Converter");
    parser.addVersionOption();

    QCommandLineOption LatOption("latitude","GPS Location <latitude>.","latitude");
    parser.addOption(LatOption);

    QCommandLineOption LonOption("longitude","GPS Location <longitude>.","longitude");
    parser.addOption(LonOption);

    QCommandLineOption ZoomOption("zoom","Zoom Value <zoom>","zoom");
    parser.addOption(ZoomOption);

    QCommandLineOption OutputOption("output","Output type <csv|json|xml>","output");
    parser.addOption(OutputOption);

    QCommandLineOption DoDebug("debug",
    QCoreApplication::translate("main", "Show Debug Details"));
    parser.addOption(DoDebug);

    QCommandLineOption ShowHelp(QStringList() << "h" << "?" << "help","Show Help Details.");
    parser.addOption(ShowHelp);

    parser.process(a);

    lat = parser.value(LatOption).toDouble();
    lon = parser.value(LonOption).toDouble();
    zoom = parser.value(ZoomOption).toDouble();
    output = parser.value(OutputOption);
    debug = parser.isSet(DoDebug);
    help = parser.isSet(ShowHelp);

    if(help)
    {
        parser.showHelp();
        exit(0);
    }

    if( !parser.isSet(LatOption) )
    {
        out << "ERROR: Lat value missing" << Qt::endl;
        exit(1);
    }

    if( !parser.isSet(LonOption) )
    {
        out << "ERROR: Lon value missing" << Qt::endl;
        exit(1);
    }

    if( !parser.isSet(ZoomOption) )
    {
        out << "ERROR: Zoom value missing" << Qt::endl;
        exit(1);
    }

    if(debug)
    {
        out << "Latitude: " << lat << Qt::endl;
        out << "Longitude: " << lon << Qt::endl;
        out << "Zoom: " << zoom << Qt::endl;
    }

    //Latitude ranges between -90 and 90 degrees, inclusive
    //Longitude ranges between -180 and 180 degrees, inclusive.

    if (lat < -90 || lat > 90 )
    {
        out << "ERROR: Latitude out of range" << Qt::endl;
        exit(1);
    }

    if (lon < -180 || lon > 180 )
    {
        out << "ERROR: Longitude out of range" << Qt::endl;
        exit(1);
    }

    if (zoom < 3 || zoom > 20 )
    {
        out << "ERROR: Zoom out of range" << Qt::endl;
        exit(1);
    }

    //(Math.floor((lon+180)/360*Math.pow(2,zoom))); }
    xtile = qFloor( ( lon + 180 ) / 360 * qPow(2,zoom) );

    xtile_string = QString("%1").arg(xtile, 0, 'f', 0);

    if(debug)
    {
        out << "xtile: " << xtile_string << Qt::endl;
    }

    //(( (1-Math.log(Math.tan(lat*Math.PI/180) + 1/Math.cos(lat*Math.PI/180))/Math.PI)/2 *Math.pow(2,zoom) ));
    ytile = qFloor( ( 1 - qLn( qTan(lat * M_PI / 180 ) + 1 / qCos( lat * M_PI/180) ) / M_PI ) /2 * qPow(2,zoom) );

    ytile_string = QString("%1").arg(ytile, 0, 'f', 0);

    if(debug)
    {
        out << "ytile: " << ytile_string << Qt::endl;
    }

    if( output.toUpper() == "JSON" )
    {
        out << "{";
        //out << "\"latitude\":\"" << lat << "\",";
        //out << "\"longitude\":\"" << lon << "\",";
        out << "\"x-tile\":\"" << xtile_string << "\",";
        out << "\"y-tile\":\"" << ytile_string << "\",";
        out << "\"zoom\":\"" << zoom << "\"";
        out << "}" << Qt::endl;

    } else if (output.toUpper() == "XML")
    {
        out << "<?xml version=\"1.0\"?>";
        out << "<Location>";
        //out << "<Latitude>" << lat << "</Latitude>";
        //out << "<Longitude>" << lon << "</Longitude>";
        out << "<ZoomLevel>" << zoom << "</ZoomLevel>";
        out << "<X-Tile>" << xtile_string << "</X-Tile>";
        out << "<Y-Tile>" << ytile_string << "</Y-Tile>";
        out << "</Location>" << Qt::endl;

    } else {

        out << xtile_string << "," << ytile_string << "," << zoom << Qt::endl;
    }

    a.exit(0);
}

// /mnt/tiles/gps2leaflet --latitude -17.291237620233325 --longitude 177.04899731622493 --zoom 20
