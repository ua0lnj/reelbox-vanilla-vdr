/***************************************************************************
 *   Copyright (C) 2005 by Reel Multimedia                                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <cstring>

#include <vdr/plugin.h>
#include <vdr/remote.h>

#include "menuBrowserBase.h"
#include "stringtools.h"
#include "dirType.h"
#include "filetools.h"


//----------cDirType-----------------------

cDirType::cDirType(edirtype dtype)
{
    GetDirType(dtype);
}

cDirType::cDirType(std::string path)
{
    GetDirType(path);
}

edirtype cDirType::GetType() const
{
    return type_->GetType();
}

std::string cDirType::GetSymbol() const
{
    return type_->GetSymbol();
}

eOSState cDirType::Open(std::string path, cMenuFileBrowserBase *menu)
{
    return type_->Open(path, menu);
}

eOSState cDirType::Play(std::string path, cMenuFileBrowserBase *menu)
{
    return type_->Play(path, menu);
}

void cDirType::GetDirType(std::string path)
{
    //printf (" cDirType::GetDirType %s \n", path.c_str());
    if(cDirTypeBaseVdrRec::IsVdrRec(path))
        type_ = &cDirTypeBaseVdrRec::Instance();
    else if(cDirTypeBaseDvd::IsDvd(path))
        type_ = &cDirTypeBaseDvd::Instance();
    else if(cDirTypeBaseDvdDir::IsDvdDir(path))
        type_ = &cDirTypeBaseDvdDir::Instance();
    else
        type_ = &cDirTypeBase::Instance();
}

void cDirType::GetDirType(edirtype type)
{
    //printf (" cDirType::GetDirType %d \n", type);
    if(type == tp_vdr_rec)
        type_ = &cDirTypeBaseVdrRec::Instance();
    else if(type == tp_dvd)
        type_ = &cDirTypeBaseDvd::Instance();
    else if(type == tp_dvd_dir)
	type_ = &cDirTypeBaseDvdDir::Instance();
    else
        type_ = &cDirTypeBase::Instance();
}

//----------cDirTypeBase-----------------------

cDirTypeBase cDirTypeBase::instance;

std::string cDirTypeBase::GetSymbol() const
{
    return "\x82\t";
}

eOSState cDirTypeBase::Open(std::string path, cMenuFileBrowserBase *menu) const
{
    return menu->EnterSubDir(path);
}

eOSState cDirTypeBase::Play(std::string path, cMenuFileBrowserBase *menu) const
{
    return Open(path, menu);
}

//----------cDirTypeBaseVdrRec---------------------------

cDirTypeBaseVdrRec cDirTypeBaseVdrRec::instance;

eOSState cDirTypeBaseVdrRec::Open(std::string path, cMenuFileBrowserBase *menu) const
{
    return menu->EnterSubDir(path);
}

eOSState cDirTypeBaseVdrRec::Play(std::string path, cMenuFileBrowserBase *menu)  const
{
    cRemote::CallPlugin("extrecmenu");
    return osEnd;
}

bool cDirTypeBaseVdrRec::IsVdrRec(std::string path)
{
    if(path != "/media/video")
        return false;
    return true;
}

//----------cDirTypeBaseDvd---------------------------
cDirTypeBaseDvd cDirTypeBaseDvd::instance;

eOSState cDirTypeBaseDvd::Open(std::string path, cMenuFileBrowserBase *menu)  const
{
    return menu->EnterSubDir(path);
}

eOSState cDirTypeBaseDvd::Play(std::string path, cMenuFileBrowserBase *menu) const
{
    return menu->EnterSubDir(path);
    //printf("----- call mediad-----\n");
    //cRemote::CallPlugin("mediad");
    //return osEnd;
}

bool cDirTypeBaseDvd::IsDvd(std::string path)
{
    if(path != "/media/dvd")
        return false;
    return true;
}

//----------cDirTypeBaseDvdDir---------------------------

cDirTypeBaseDvdDir cDirTypeBaseDvdDir::instance;

std::string cDirTypeBaseDvdDir::GetSymbol() const
{
    return "\x81\t";
}

eOSState cDirTypeBaseDvdDir::Open(std::string path, cMenuFileBrowserBase *menu) const
{
    return menu->EnterSubDir(path);
}

eOSState cDirTypeBaseDvdDir::Play(std::string path, cMenuFileBrowserBase *menu) const
{
    if(strcmp(GetLast(path).c_str(), "VIDEO_TS"))
	path += "/VIDEO_TS";

    const cPlayListItem *item = new cPlayListItem(path);
    return menu->ShowDvdWithXine(*item, false);
}

bool cDirTypeBaseDvdDir::IsDvdDir(std::string path)
{
    /* directory itself is a "VIDEO_TS"-directory */
    if(!strcmp(GetLast(path).c_str(), "VIDEO_TS")){
	//printf("\"%s\" IS DVD DIR :-)\n", path.c_str());
        return true;
    }

    std::string test_path;
    test_path = path + "/VIDEO_TS";
    if (access(test_path.c_str(), R_OK|X_OK) == 0)
        return true;
    test_path = path + "/video_ts";
    if (access(test_path.c_str(), R_OK|X_OK) == 0)
        return true;

    return false;
}
