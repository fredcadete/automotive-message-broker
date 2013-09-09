/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ABSTRACTPROPERTY_H
#define ABSTRACTPROPERTY_H

#include <string>
#include <functional>
#include <boost/any.hpp>
#include <glib.h>

#include "debugout.h"
#include "abstractpropertytype.h"
#include "abstractroutingengine.h"
#include "vehicleproperty.h"

class AbstractDBusInterface;

using namespace std;

typedef function<void (boost::any)> SetterFunc;

class AbstractProperty
{

public:

	enum Access {
		Read,
		Write,
		ReadWrite
	};

	AbstractProperty(string propertyName, string signature, Access access, AbstractDBusInterface* interface);
	
	virtual void setSetterFunction(SetterFunc setterFunc)
	{
		mSetterFunc = setterFunc;
	}
	
	virtual string signature()
	{
		return mSignature;
	}
	
	virtual string name() 
	{
		return mPropertyName;
	}

	virtual VehicleProperty::Property ambPropertyName()
	{
		return mAmbPropertyName;
	}

	virtual Access access()
	{
		return mAccess;
	}

	void setSourceFilter(std::string filter) { mSourceFilter = filter; }
	void setZoneFilter(Zone::Type zone) { mZoneFilter = zone; }
	
	virtual GVariant* toGVariant() = 0;
	virtual void fromGVariant(GVariant *value) = 0;

	double timestamp()
	{
		return mTimestamp;
	}

	int sequence()
	{
		if(mValue)
			return mValue->sequence;
		return 0;
	}

	virtual void setValue(AbstractPropertyType* val)
	{
		if(mValue) delete mValue;

		PropertyInfo info = routingEngine->getPropertyInfo(val->name, val->sourceUuid);

		mValue = val->copy();
		mAnyValue = val->anyValue();
		mTimestamp = val->timestamp;
		if(info.isValid())
			mUpdateFrequency = info.updateFrequency();
		updateValue();
	}

	int updateFrequency()
	{
		return mUpdateFrequency;
	}

	/*template<typename T>
	void setValue(T val)
	{
		mAnyValue = val;
		updateValue();
	}*/



	template<typename T>
	T anyValue()
	{
		return boost::any_cast<T>(mAnyValue);
	}

	AbstractPropertyType* value()
	{
		return mValue;
	}
	
protected: ///methods:

	void updateValue();

	std::string mSourceFilter;
	Zone::Type mZoneFilter;
	int mUpdateFrequency;
	AbstractRoutingEngine* routingEngine;
	
protected:
	
	boost::any mAnyValue;
	string mPropertyName;
	VehicleProperty::Property mAmbPropertyName;
	string mSignature;
	SetterFunc mSetterFunc;
	Access mAccess;
	double mTimestamp;
	AbstractPropertyType* mValue;
	AbstractDBusInterface* mInterface;
};

#endif // ABSTRACTPROPERTY_H
