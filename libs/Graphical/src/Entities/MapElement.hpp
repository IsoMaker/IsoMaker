/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** MapTile
*/

#pragma once

#include "Asset3D.hpp"
#include "AEntity.hpp"

namespace objects
{
    class MapElement : public AEntity
    {
        public:
            MapElement(Asset2D asset2D, Asset3D asset3D);
            MapElement(Asset2D asset2D, Asset3D asset3D, Vector3D position);
            MapElement(Asset2D asset2D, Asset3D asset3D, Vector3D position, Vector3D size, float scale);

            ~MapElement() = default;

            Asset3D getAsset3D() const;
            void setAsset3D(Asset3D asset3D);

            void draw() override;

        protected:
            Asset3D _asset3D;

        private:
    };
}
