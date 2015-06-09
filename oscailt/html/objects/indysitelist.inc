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

require_once 'objects/omlquery.inc';


class IndySite
{
   var $name;
   var $url;
   var $region;
}

class IndyRegion
{
   var $name;
   var $sites = array();
}

class IndySiteList
{
   var $regions = array();
   var $sites = array();
   var $lastModified;
   var $convert_from_utf8 = false;

   function loadList($query, $expiry)
   {
      global $query_cache,$system_config;
      $cachedlist=$query_cache->getCachedData($query,"oml");
      if($cachedlist!=null)
      {
         $this->regions=$cachedlist->regions;
         $this->sites=$cachedlist->sites;
         $this->lastModified=$cachedlist->lastModified;
         $cachedtime = $query_cache->getCachedTime($query,"oml");
         if(time()-$cachedtime<$expiry) return;
      }
      // no cache or cache timed out
      $omlquery = new OMLQuery($this->convert_from_utf8);
      // Set this to force a nice display of error messages during full install
      if (isset($_REQUEST['dataobjects_caching_done'])) $omlquery->setInstallMode();

      $oml = $omlquery->fetchParsedOML($query);
      if($oml==null)
      {
         //error so retouch cache and wait till next interval
         $query_cache->touchCachedData($query,"oml");
         return;
      }
      else
      {
         $this->regions=array();
         $this->sites=array();
         $this->lastModified=null;
         $projects= new IndyRegion();
         $projects->name="projects";
         $process= new IndyRegion();
         $process->name="process";
         foreach ($oml->outlines as $outline)
         {
            if($outline->attributes["TYPE"]=="list" && $outline->attributes["TEXT"]=="Indymedia")
            {
               $this->lastModified=strtotime($outline->attributes["MODIFIED"]);
               foreach ($outline->outlines as $suboutline)
               {
                  if($suboutline->attributes["TYPE"]=="link")
                  {
                     $indysite = new IndySite();
                     $indysite->name=$suboutline->attributes["TEXT"];
                     if($indysite->name=="www.indymedia.org") continue;
                     $indysite->url=$suboutline->attributes["URL"];
                     foreach ($suboutline->items as $item)
                     {
                        if($item->attributes["NAME"]=="region")
                        {
                           $indysite->region = trim($item->cdata);
                           break;
                        }
                     }
                     if($indysite->region=="" || $indysite->region==null)
                     {
                        switch(strtolower($indysite->name))
                        {
                           case "discussion":
                           case "fbi/legal updates":
                           case "indymedia faq":
                           case "mailing lists":
                           case "process & imc docs":
                           case "process & imc docs":
                           case "tech":
                           case "volunteer":
                              $indysite->region="process";
                              break;
                           case "oceania":
                              $indysite->region="oceania";//oceania regional site
                              break;
                           default:
                              $indysite->region="projects";
                              break;
                        }
                     }
                     array_push($this->sites,$indysite);
                     if($indysite->region=="process") array_push($process->sites,$indysite);
                     else if($indysite->region=="projects") array_push($projects->sites,$indysite);
                     else
                     {
                        $indyregion=$this->regions[$indysite->region];
                        if($indyregion==null)
                        {
                           $indyregion=new IndyRegion();
                           $indyregion->name=$indysite->region;
                        }
                        array_push($indyregion->sites,$indysite);
                        $this->regions[$indyregion->name]=$indyregion;
                     }
                  }
               }
            }
         }
         if(count($this->sites)==0)
         {
            $this->regions=$cachedlist->regions;
            $this->sites=$cachedlist->sites;
            $this->lastModified=$cachedlist->lastModified;
         }
         else
         {
            ksort($this->regions);
            $this->regions["process"]=$process;
            $this->regions["projects"]=$projects;
         }
         $query_cache->cacheData($query,$this,"oml");
      }
   }

   function addGlobalRegion()
   {
      $glob = new IndyRegion();
      $glob->name="Global";
      $indysite = new IndySite();
      $indysite->name = "global site";
      $indysite->url = "http://www.indymedia.org";
      $indysite->region = "Global";
      $glob->sites[] = $indysite;
      array_unshift($this->regions, $glob);
   }
}
?>
