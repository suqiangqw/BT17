#ifndef MODEL_H
#define MODEL_H

#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QNormalDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QMorphPhongMaterial>

class Model
{
public:
    Model(Qt3DCore::QEntity *sceneRoot);

public slots:
    void Initialize();
    void SetCenterRotation(QQuaternion quaternion);
    void SetLeftUpperArmRotation(QQuaternion quaternion);
    void SetLeftLowerArmRotation(QQuaternion quaternion);
    void SetRightUpperArmRotation(QQuaternion quaternion);
    void SetRightLowerArmRotation(QQuaternion quaternion);
    void SetLeftUpperLegRotation(QQuaternion quaternion);
    void SetLeftLowerLegRotation(QQuaternion quaternion);
    void SetRightUpperLegRotation(QQuaternion quaternion);
    void SetRightLowerLegRotation(QQuaternion quaternion);
    void SetHeadRotation(QQuaternion quaternion);

private:
    Qt3DCore::QEntity *sceneRoot;

    //transforms
    Qt3DCore::QTransform *centerTransform;
    Qt3DCore::QTransform *leftLowerArmTransform;
    Qt3DCore::QTransform *leftUpperArmTransform;
    Qt3DCore::QTransform *leftUpperLegTransform;
    Qt3DCore::QTransform *leftLowerLegTransform;
    Qt3DCore::QTransform *rightLowerArmTransform;
    Qt3DCore::QTransform *rightUpperArmTransform;
    Qt3DCore::QTransform *rightUpperLegTransform;
    Qt3DCore::QTransform *rightLowerLegTransform;
    Qt3DCore::QTransform *headTransform;

    QVector4D topCenterOrigin;
    QVector4D bottomCenterOrigin;
    QVector4D connectorLeftUpperAndLowerArmOrigin;
    QVector4D connectorRightUpperAndLowerArmOrigin;
    QVector4D connectorLeftUpperAndLowerLegOrigin;
    QVector4D connectorRightUpperAndLowerLegOrigin;

    QMatrix4x4 luaOffsetRM;
    QMatrix4x4 llaOffsetRM;
    QMatrix4x4 ruaOffsetRM;
    QMatrix4x4 rlaOffsetRM;
    QMatrix4x4 lulOffsetRM;
    QMatrix4x4 lllOffsetRM;
    QMatrix4x4 rulOffsetRM;
    QMatrix4x4 rllOffsetRM;
    QMatrix4x4 offsetTM;
    QMatrix4x4 headOffsetTM;
    QMatrix4x4 headTM;
    QMatrix4x4 topCenterTM;
    QMatrix4x4 bottomCenterTM;
    QMatrix4x4 connectorLeftUpperAndLowerArmTM;
    QMatrix4x4 connectorRightUpperAndLowerArmTM;
    QMatrix4x4 connectorLeftUpperAndLowerLegTM;
    QMatrix4x4 connectorRightUpperAndLowerLegTM;

private slots:
    QMatrix4x4 GetTranslationMatrix(float x, float y, float z);
};

#endif // MODEL_H