#ifndef INCLUDED_CORE_INVENTORY_COMPONENT_H
#define INCLUDED_CORE_INVENTORY_COMPONENT_H
#include "core/i_inventory_component.h"
#include "core/item_factory.h"
#include "core/property_loader.h"

class InventoryComponent : public IInventoryComponent
{
public:
    virtual ItemList_t const& GetItems()const;
    virtual void AddItem( int32_t Id );
    virtual void DropItemType( Item::ItemType Type );
    virtual void Update( double Seconds );
protected:
    InventoryComponent();
    friend class ComponentFactory;
private:
    ItemFactory& mItemFactory;
    ItemList_t mItems;
};

class InventoryComponentLoader: public ComponentLoader<InventoryComponent>
{
    virtual void BindValues();
protected:
    InventoryComponentLoader();
    friend class ComponentLoaderFactory;
};

#endif//INCLUDED_CORE_INVENTORY_COMPONENT_H