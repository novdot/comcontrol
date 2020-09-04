#ifndef ProtocolInterface_H
#define ProtocolInterface_H

#include <QObject>
#include <QWidget>
#include <QPair>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <QTimer>


/*!
    \defgroup ProtocolInterface интерфейс устройства
    \ingroup Protocol
    \brief интерфейс устройства
*/
///@{


///Форматы информации
typedef enum protocol_formatDef{
    _protocol_format_hex = 0 ///< информация - строка содержащая в 16-ой код
    , _protocol_format_utf8 = 1 ///< информация тестовой формат в кодировке UTF-8
    , _protocol_format_ascii = 2 ///< информация тестовой формат в кодировке ASCII
}protocol_format;

#define PROTOCOL_ROBOT_VALUE_DEFAULT "as is"

///структура элемента робота
typedef struct protocol_robot_srcDef{
    void* device; ///< ссылка на устройство Protocol
    protocol_format format; ///< формат информации
    QString value; ///< значение
}protocol_robot_src;

///структура условия событие -> действие
typedef struct protocol_robotDef {
    QString name;  ///< имя условия
    protocol_robot_src slave;  ///< элемент раб
    protocol_robot_src master;  ///< элемент мастер
}protocol_robot;

/*!
    \brief
    \author dnovikov
    \date 2020 01 17
    \warning на стадии разработки
 * */
class ProtocolInterface : public QWidget
{
    Q_OBJECT
public:
    ProtocolInterface(QWidget *parent = nullptr): QWidget(parent) {
        m_lFormat = ProtocolInterface::getFormatList();

        tim = new QTimer();
        connect(tim,SIGNAL(timeout()),SLOT(timUpdateEvent()));
    }
    /*!
        \brief получение текущего списка условий для робота
     * */
    QList<protocol_robot> getRobotList(){return m_lRobot;}
    /*!
        \brief установка списка условий для робота
     * */
    void setRobotList(QList<protocol_robot> list){
        m_lRobot = list;

        //проверка на элементы , где есть только слейв. занесем их в отдельный список
        QList<protocol_robot>::ConstIterator begin;
        QList<protocol_robot>::ConstIterator end;
        begin = m_lRobot.constBegin();
        end = m_lRobot.constEnd();

        m_lRobotSlave.clear();

        for(QList<protocol_robot>::ConstIterator i = begin; i != end; ++i){
            if( ((*i).master.device != nullptr) ) continue;
            m_lRobotSlave.append((*i));
        }
        updateSlaveControl();
    }

    /*!
        \brief получить список всех возможных форматов
     * */
    static QList< QPair<int,QString> > getFormatList(){
        QList< QPair<int,QString> > lFormat;
        QPair<int,QString> format;
        format.first = _protocol_format_hex;
        format.second = trUtf8("Hex");
        lFormat.append(format);
        format.first = _protocol_format_utf8;
        format.second = trUtf8("Utf8");
        lFormat.append(format);
        format.first = _protocol_format_ascii;
        format.second = trUtf8("ASCII");
        lFormat.append(format);

        return lFormat;
    }

public slots:
    /*!
     * \brief timStop остановка таймера
     */
    virtual void timStart(int period){tim->start(period);}
    /*!
     * \brief timStop остановка таймера
     */
    virtual void timStop(){tim->stop();}
    /*!
     * \brief timUpdateEvent слот обработки событий таймера
     */
    virtual void timUpdateEvent(){}
    /*!
     * \brief слот для обработки входящего пакета
    */
    virtual void receiveDataFromDevice(QByteArray a_data){Q_UNUSED(a_data)}
    /*!
     * \brief слот для отправки пакета
    */
    virtual void sendDataToDevice(QByteArray a_data){Q_UNUSED(a_data)}
    /*!
     * \brief обновление отображения логики робота
     */
    virtual void showRobotView(){}
    /*!
     * \brief обновление списка возможных команд
     */
    virtual void updateSlaveControl(){}
    /*!
     * \brief слот для обновления имени устройства в случае подключения устройства
    */
    void itemConnected(void * a_pitem,QString a_strName){m_strCOMName=a_strName;}
    /*!
     * \brief слот для сброса имени устройства в случае отключения устройства
    */
    void itemDisconnect(void * a_pitem){m_strCOMName="";}
    /*!
     * \brief получить текущее имя устройства
    */
    QString getCOMName(){return m_strCOMName;}

signals:
    /*!
     * \brief сигнал пакета управления устройством
    */
    void send(QByteArray);

    /*!
     * \brief сигнал обновления списка слейва, для обновления возможных команд от Protocol
    */
    void slaveListUpdated();

    /*!
     * \brief receive сигнал о полученном пакете
     */
    void receiveDone(QByteArray);

    /*!
     * \brief Отправка в лог информации
     */
    void add2Log(QString);
private:
protected:
    QString m_strCOMName; ///< имя устройства
    QList<protocol_robot> m_lRobot; ///< список событий
    QList<protocol_robot> m_lRobotSlave;///< подсписки событий. Содержатся только события для слейва - быстрые команды на выдачу
    QList< QPair<int,QString> > m_lFormat; ///< списки возможных значений: формат входных/выходных данных   
    QTimer *tim; ///< таймер для периодической отправки
};

///@}
#endif // ProtocolInterface_H