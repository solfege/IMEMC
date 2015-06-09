<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

require_once($path_prefix."objects/feedexport/feed.inc");
require_once($path_prefix."objects/feedexport/rssfeedexporter.inc");
require_once($path_prefix."objects/feedexport/atomfeedexporter.inc");
require_once($path_prefix."objects/feedexport/jsfeedexporter.inc");

class FeedExporter
{
   /* This structure encodes the difference between ISO-8859-1 and Windows-1252,
      as a map from the UTF-8 encoding of some ISO-8859-1 control characters to
      the UTF-8 encoding of the non-control characters that Windows-1252 places
      at the equivalent code points. */
   var $cp1252_map = array(
      "\xc2\x80" => "\xe2\x82\xac", /* EURO SIGN */
      "\xc2\x82" => "\xe2\x80\x9a", /* SINGLE LOW-9 QUOTATION MARK */
      "\xc2\x83" => "\xc6\x92",    /* LATIN SMALL LETTER F WITH HOOK */
      "\xc2\x84" => "\xe2\x80\x9e", /* DOUBLE LOW-9 QUOTATION MARK */
      "\xc2\x85" => "\xe2\x80\xa6", /* HORIZONTAL ELLIPSIS */
      "\xc2\x86" => "\xe2\x80\xa0", /* DAGGER */
      "\xc2\x87" => "\xe2\x80\xa1", /* DOUBLE DAGGER */
      "\xc2\x88" => "\xcb\x86",    /* MODIFIER LETTER CIRCUMFLEX ACCENT */
      "\xc2\x89" => "\xe2\x80\xb0", /* PER MILLE SIGN */
      "\xc2\x8a" => "\xc5\xa0",    /* LATIN CAPITAL LETTER S WITH CARON */
      "\xc2\x8b" => "\xe2\x80\xb9", /* SINGLE LEFT-POINTING ANGLE QUOTATION */
      "\xc2\x8c" => "\xc5\x92",    /* LATIN CAPITAL LIGATURE OE */
      "\xc2\x8e" => "\xc5\xbd",    /* LATIN CAPITAL LETTER Z WITH CARON */
      "\xc2\x91" => "\xe2\x80\x98", /* LEFT SINGLE QUOTATION MARK */
      "\xc2\x92" => "\xe2\x80\x99", /* RIGHT SINGLE QUOTATION MARK */
      "\xc2\x93" => "\xe2\x80\x9c", /* LEFT DOUBLE QUOTATION MARK */
      "\xc2\x94" => "\xe2\x80\x9d", /* RIGHT DOUBLE QUOTATION MARK */
      "\xc2\x95" => "\xe2\x80\xa2", /* BULLET */
      "\xc2\x96" => "\xe2\x80\x93", /* EN DASH */
      "\xc2\x97" => "\xe2\x80\x94", /* EM DASH */

      "\xc2\x98" => "\xcb\x9c",    /* SMALL TILDE */
      "\xc2\x99" => "\xe2\x84\xa2", /* TRADE MARK SIGN */
      "\xc2\x9a" => "\xc5\xa1",    /* LATIN SMALL LETTER S WITH CARON */
      "\xc2\x9b" => "\xe2\x80\xba", /* SINGLE RIGHT-POINTING ANGLE QUOTATION*/
      "\xc2\x9c" => "\xc5\x93",    /* LATIN SMALL LIGATURE OE */
      "\xc2\x9e" => "\xc5\xbe",    /* LATIN SMALL LETTER Z WITH CARON */
      "\xc2\x9f" => "\xc5\xb8"      /* LATIN CAPITAL LETTER Y WITH DIAERESIS*/
   );

   var $self_lnk;

   function FeedExporter($l)
   {
      $this->self_lnk = $l;
   }

   function convertToUnicode($str) {
          return  strtr(utf8_encode($str), $this->cp1252_map);
   }

   // formats date in W3CDTF (YYYY-MM-DDThh:mm:ssTZD) a profile of ISO 8601
   // sepate fxn required because php 4 date fxn doesn't support it (php 5 does)
   function formatW3CDate($date,$timezoneOffset)
   {
      //adjust local hosts timezone gmt offset to the gmt offset of our  timezone
      $gmtOffset=sprintf("%+05d",  date("O")+(($timezoneOffset/60/60)*100));
      // insert colon to match W3C date standard
      $gmtOffset =  $gmtOffset[0].$gmtOffset[1].$gmtOffset[2].":".$gmtOffset[3].$gmtOffset[4];
      return date("Y-m-d\TH:i:s",$date+$timezoneOffset).$gmtOffset;
   }

   function formatISO822Date($date,$timezoneOffset)
   {
      //adjust local hosts timezone gmt offset to the gmt offset of our  timezone
      $gmtOffset=sprintf("%+05d",  date("O")+(($timezoneOffset/60/60)*100));
      return date("D, d M Y H:i:s ",$date+$timezoneOffset).$gmtOffset;
   }

   function getContentType()
   {
      return "NOT DEFINED";
   }

   function export($feed)
   {
      return "NOT DEFINED";
   }
}

