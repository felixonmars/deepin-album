/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     ZhangYong <zhangyong@uniontech.com>
 *
 * Maintainer: ZhangYong <ZhangYong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DBMANAGER_H
#define DBMANAGER_H

// ImageTable
///////////////////////////////////////////////////////
//FilePath           | FileName | Dir        | Time | ChangeTime //
//TEXT primari key   | TEXT     | TEXT       | TEXT | TEXT //
///////////////////////////////////////////////////////

// AlbumTable
/////////////////////////////////////////////////////
//AP               | AlbumName         | FilePath  //
//TEXT primari key | TEXT              | TEXT      //
/////////////////////////////////////////////////////

#include <QObject>
#include <QDateTime>
#include <QMutex>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <mutex>
#include "albumgloabl.h"
//#include "connectionpool.h"

const QString DATETIME_FORMAT_DATABASE = "yyyy.MM.dd hh:mm";

enum AlbumDBType {
    Favourite,
    Custom,
    AutoImport,
    TypeCount
};

class QSqlDatabase;

//注意：需要支持相册重名的版本，在对底层相册操作时，只能传入UID

class DBManager : public QObject
{
    Q_OBJECT
public:
    //特殊UID
    //-1,不属于任意相册
    //0,我的收藏
    //1,监控路径：Camera
    //2,监控路径：Screen Capture
    //3,监控路径：Draw

    enum SpUID {
        u_NotInAnyAlbum = -1,
        u_Favorite,
        u_ScreenCapture,
        u_Camera,
        u_Draw,
        u_CustomStart
    };

    static DBManager  *instance();
    explicit DBManager(QObject *parent = nullptr);
    ~DBManager()
    {
//        ConnectionPool::release(); //释放数据库连接
    }

    // TableImage
    const QStringList       getAllPaths() const;
    const DBImgInfoList     getAllInfos(int loadCount = 0) const;
    const QStringList       getAllTimelines() const;
    const DBImgInfoList     getInfosByTimeline(const QString &timeline) const;
    const QStringList       getImportTimelines() const;
    const DBImgInfoList     getInfosByImportTimeline(const QString &timeline) const;
//    const DBImgInfo         getInfoByName(const QString &name) const;
    const DBImgInfo         getInfoByPath(const QString &path) const;
//    const DBImgInfo         getInfoByPathHash(const QString &pathHash) const;
    int                     getImgsCount() const;
//    bool                    isImgExist(const QString &path) const;
    void                    insertImgInfos(const DBImgInfoList &infos);
    void                    insertImgInfo(const DBImgInfo &info);
    void                    removeImgInfos(const QStringList &paths);
    void                    removeImgInfosNoSignal(const QStringList &paths);
    const DBImgInfoList     getInfosForKeyword(const QString &keywords) const;
    const DBImgInfoList     getTrashInfosForKeyword(const QString &keywords) const;
    const DBImgInfoList     getInfosForKeyword(int UID, const QString &keywords) const;

    //CustomAutoImportPathTable
    //检查当前的自定义自动导入路径是否已经被监控，检查内容包括是否是子文件夹和是否是默认导入路径
    bool checkCustomAutoImportPathIsNotified(const QString &path);
    //添加新的自定义自动导入路径，步骤类似于新建相册，传入路径和相册名，然后返回UID
    int createNewCustomAutoImportPath(const QString &path, const QString &albumName);
    //删除自定义自动导入路径，需要以UID为删除依据
    void removeCustomAutoImportPath(int UID);
    //获取所有需要监控的路径
    std::map<int, QString> getAllCustomAutoImportUIDAndPath();

    // TableAlbum
    const QMultiMap<QString, QString> getAllPathAlbumNames() const;
    //输入：相册类型，输出：所属类型下的相册UID、相册名称
    const QList<std::pair<int, QString> > getAllAlbumNames(AlbumDBType atype = AlbumDBType::Custom) const;
    //从UID判断是否是默认导入路径
    static bool isDefaultAutoImportDB(int UID);
    //获取默认监控路径数据包，数据顺序是：监控路径，界面显示名字，对应的UID
    static std::tuple<QStringList, QStringList, QList<int> > getDefaultNotifyPaths();
    //获取默认监控路径数据包，数据顺序是：监控路径，界面显示名字，对应的UID，和不带_group的区别是，它把路径打包起来了
    static std::tuple<QList<QStringList>, QStringList, QList<int>> getDefaultNotifyPaths_group();
    //确认目标UID对应的默认监控路径是否存在
    static bool defaultNotifyPathExists(int UID);
    const QStringList       getPathsByAlbum(int UID) const;
    const DBImgInfoList     getInfosByAlbum(int UID) const;
    int                     getItemsCountByAlbum(int UID, const ItemType &type) const;
//    int                     getAlbumsCount() const;
    bool                    isAlbumExistInDB(int UID, AlbumDBType atype = AlbumDBType::Custom) const;
    QString                 getAlbumNameFromUID(int UID) const;
    AlbumDBType             getAlbumDBTypeFromUID(int UID) const;
    bool                    isAllImgExistInAlbum(int UID, const QStringList &paths, AlbumDBType atype = AlbumDBType::Custom) const;
    bool                    isImgExistInAlbum(int UID, const QString &path, AlbumDBType atype = AlbumDBType::Custom) const;
    bool                    insertIntoAlbum(int UID, const QStringList &paths, AlbumDBType atype = AlbumDBType::Custom);
    int                     createAlbum(const QString &album, const QStringList &paths, AlbumDBType atype = AlbumDBType::Custom);
    void                    removeAlbum(int UID);
    void                    removeFromAlbum(int UID, const QStringList &paths, AlbumDBType atype = AlbumDBType::Custom);
    void                    renameAlbum(int UID, const QString &newAlbum, AlbumDBType atype = AlbumDBType::Custom);
    // TabelTrash
    const DBImgInfoList     getAllTrashInfos() const;
    void                    insertTrashImgInfos(const DBImgInfoList &infos);
    void                    removeTrashImgInfos(const QStringList &paths);
    QStringList             recoveryImgFromTrash(const QStringList &paths); //返回无法恢复的文件
    void                    removeTrashImgInfosNoSignal(const QStringList &paths);
    const DBImgInfo         getTrashInfoByPath(const QString &path) const;
    const DBImgInfoList     getTrashImgInfos(const QString &key, const QString &value) const;
    int                     getTrashImgsCount() const;
    int                     getAlbumImgsCount(int UID) const;
private:
    const DBImgInfoList     getInfosByNameTimeline(const QString &value) const;
    const DBImgInfoList     getImgInfos(const QString &key, const QString &value) const;

    void                    checkDatabase();
    static DBManager       *m_dbManager;
    static std::once_flag   instanceFlag; //线程安全的单例flag
    void insertSpUID(const QString &albumName, AlbumDBType astype, SpUID UID);
private:
    mutable QMutex m_mutex;
    mutable QSqlQuery *m_query; //将数据库查询对象统一到类成员变量，以尝试解决sqlite崩溃问题
    std::atomic_int albumMaxUID; //当前数据库中UID的最大值，用于新建UID用
};

#endif // DBMANAGER_H
