#!/usr/bin/env python

from sakusen import *
from sakusen_resources import *

if(__name__=="__main__"):
	#setup resource interface
	spath = fileUtils_getHome()
	spath /= path(CONFIG_SUBDIR)
	spath /= path(DATA_SUBDIR)
	resint = FileResourceInterface_create(spath, False)
	
	laser = WeaponType("laser", "sagittarius", "o", 0, 0, 0, 0, resint)
	torpedo = WeaponType("torpedo", "sagittarius", "c:torp", 0, 0, 0, 0, resint)
	

	
	CM = 100000
	
	s = Sensors()
	s.optical.capable = True
	s.optical.range = 45 * CM #45cm
	
	torps = Sensors()
	torps.optical.capable = True
	torps.optical.range = 5 * CM
	#dex ~= micrometres in BFG
	# typical ship: 3000 hp # 30000
	# typical ship fires 30 times in 10000 # no, once per tick with damage 30
        # i.e. once every 300 ticks 
        # therefore, typical ship takes 1000000 ticks to shoot another # no 1000
        # so let's say one BFG turn is 100000 ticks # no, 100
	# so ship should move 200000 dex in 100000 ticks # 
	# i.e. 2 dex per tick
	# so increase by 1000000 to give room
	# dex now ~= picometres
	# no, 10 picometers, because 32-bit
	# typical speed should be 200000
	# typical acceleration should be 1
	# never mind above: speed should be 20000 in 100 ticks, i.e. 200
	# accesseration should be 200 in 200 ticks, i.e. 1
	# if we want accel up to 100, must make speed up to 20000 and 1cm 100000
	
	accelrect = SRectangle16(-2, 0, 2, 100)
	accelrr = SRectangleRegion16(accelrect)
	ap = SRectangleRegion16.createRegionPtr(accelrr)
	accelrr.thisown = 0 #shared_ptr
	velrect = SSphereRegion16(SPoint16(0,0,0),20000)
	vp = SSphereRegion16.createRegionPtr(velrect)
	velrect.thisown = 0
	angularvelrect = SSphereRegion32(SPoint32(0,0,0),45)
	avp = SSphereRegion32.createRegionPtr(angularvelrect)
	angularvelrect.thisown = 0
	shiptype = UnitTypeData(HitPoints(10000), 100, UPoint32(CM // 2, CM // 10, CM // 10), ap, vp, avp, Visibility(), s)
	ship = UnitType("ship", shiptype, 0, 0, "ground", [], "")
	
	

	#make universe and save
	u = Universe("sagittarius", "", "sagittarius", "create_script", [laser, torpedo], [ship])
	err = u.resolveNames()
	print("Resolving names: %s"%err)
	u.thisown = 0 #because shared_ptr
	up = Universe.createConstPtr(u)
	res = resint.usave(up, "sagittarius")
	print("Tried to save, result: %d" % res)
