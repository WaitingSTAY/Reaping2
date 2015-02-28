#ifndef INCLUDED_CORE_ACCELERATION_COMPONENT_H
#define INCLUDED_CORE_ACCELERATION_COMPONENT_H

#include "i_move_component.h"
#include "core/property_loader.h"
#include "i_acceleration_component.h"



class AccelerationComponent : public IAccelerationComponent
{
public:
    virtual double const& GetAcceleration() const;
    virtual double const& GetMinSpeed() const;
    virtual double const& GetMaxSpeed() const;
    virtual void SetAcceleration( double acceleration );
    virtual void SetMinSpeed( double minSpeed );
    virtual void SetMaxSpeed( double maxSpeed );
protected:
    AccelerationComponent();
    friend class ComponentFactory;
private:
    double mAcceleration;
    double mMinSpeed;
    double mMaxSpeed;
};

class AccelerationComponentLoader: public ComponentLoader<AccelerationComponent>
{
    virtual void BindValues();
protected:
    AccelerationComponentLoader();
    friend class ComponentLoaderFactory;
};

#endif//INCLUDED_CORE_ACCELERATION_COMPONENT_H