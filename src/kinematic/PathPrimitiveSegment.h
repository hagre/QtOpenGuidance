// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#pragma once

#include "PathPrimitive.h"

class PathPrimitiveSegment : public PathPrimitive {
  public:
    PathPrimitiveSegment()
      : PathPrimitive() {}

    PathPrimitiveSegment( const Segment_2& segment, double implementWidth, bool anyDirection, int32_t passNumber );

    virtual Type getType() override {
      return Type::Segment;
    }

  public:
    bool operator==( PathPrimitiveSegment& b ) {
      return segment == b.segment;
    }
    bool operator==( const PathPrimitiveSegment& b ) const {
      return segment == b.segment;
    }

  public:
    virtual double distanceToPointSquared( const Point_2& point ) override;
    virtual bool isOn( const Point_2& ) override;
    virtual bool leftOf( const Point_2& point ) override;
    virtual double angleAtPointDegrees( const Point_2& ) override;

    virtual bool intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) override;
    virtual Line_2 perpendicularAtPoint( const Point_2 point )override;
    virtual Point_2 orthogonalProjection( const Point_2 point )override;

    virtual void transform( const Aff_transformation_2& transformation ) override;

    virtual std::shared_ptr<PathPrimitive> createReverse() override;
    virtual std::shared_ptr<PathPrimitive> createNextPrimitive( bool left, bool reverse ) override;

    virtual void print() override {
      std::cout << "PathPrimitiveSegment: " << segment << std::endl;
    }

  public:
    double angleLineDegrees = 0;
    Segment_2 segment;
    Line_2 supportingLine;
};
