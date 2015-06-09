<?php

/************************************************************************/

/* Oscailt                         */

/* Indepenent Media Centre Content Management System          */

/* ====================================================================   */

/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */

/* http://www.indymedia.ie                  */

/* Development List: oscailt@lists.indymedia.org                        */

/* See contributions.txt for the list of contributors                   */

/*                                                                      */

/* This program is free software. You can redistribute it and/or modify */

/* it under the terms of the GNU General Public License as published by */

/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */

/*                           */

/* This program is distributed in the hope that it will be useful,   */

/* but WITHOUT ANY WARRANTY; without even the implied warranty of   */

/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   */

/* GNU General Public License for more details.            */ 

/************************************************************************/



require_once($path_prefix."objects/feedexport/feeditem.inc");

   
class Feed

{

   var $feedLink;

   var $webLink;

   var $siteLink;

   var $logo;

   var $icon;

   var $title;

   var $description;

   var $date;

   var $timezoneOffset;

   var $publisherName;

   var $publisherLink;

   var $publisherEmail;

   var $license;

   var $languageCode;

   var $languageName;
   var $categories = array();
   var $generatorName;

   var $generatorVersion;

   var $generatorLink;

   var $items = array();   

   //iTunes tags for use only with RSS 2.0

   var $iTunesTags;

   var $iTunesImage;

   var $iTunesExplicit;

   var $iTunesCategory;

   var $iTunesSubcategory;

   var $iTunesAuthor;

   var $iTunesOwner;

   var $iTunesOwnerEmail;

   var $iTunesSummary;

   var $iTunesSubtitle;

   var $iTunesKeywords;

   var $iTunesBlock; 

}

