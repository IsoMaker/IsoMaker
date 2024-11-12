#pragma once

#include "IObject3D.hpp"
#include "../Assets/Asset2D.hpp"
#include "../Assets/Asset3D.hpp"
#include "../Utilities/ObjectBox.hpp"

using namespace Utilities;

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

namespace Object3D {

    enum AssetType {
        ASSET3D,
        ASSET2D,
    };

    class BasicObject3D : public IObject3D {
        public:
            BasicObject3D();
            BasicObject3D(Asset3D, Vector3D);
            BasicObject3D(Asset2D, Vector3D, Vector3D dimension3D);
            ~BasicObject3D();

            void setAsset(Asset3D);
            void setAsset(Asset2D, Vector3D dimension3D);
            void draw();
            void move(Vector3D);
            void moveTo(Vector3D);
            void resizePercent(float);
            ObjectBox3D getBox();

        private:
            ObjectBox3D _objectBox;
            Asset3D _asset3D;
            Asset2D _asset2D;
            AssetType _assetType;
    };  

}