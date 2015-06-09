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
/* This program is distributed in the hope that it will be useful ,     */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

/* $dbtype = "MySQL"; */
/* $dbtype = "mSQL"; */
/* $dbtype = "PostgreSQL"; */
/* $dbtype = "PostgreSQL_local";// When postmaster start without "-i" option. */
/* $dbtype = "ODBC"; */
/* $dbtype = "ODBC_Adabas"; */
/* $dbtype = "Interbase"; */
/* $dbtype = "Sybase"; */

class ResultSet {
   var $result;
   var $total_rows;
   var $fetched_rows;

   function set_result( $res ) {
      $this->result = $res;
   }

   function get_result() {
      return $this->result;
   }

   function set_total_rows( $rows ) {
      $this->total_rows = $rows;
   }

   function get_total_rows() {
      return $this->total_rows;
   }

   function set_fetched_rows( $rows ) {
      $this->fetched_rows = $rows;
   }

   function get_fetched_rows() {
      return $this->fetched_rows;
   }

   function increment_fetched_rows() {
      $this->fetched_rows = $this->fetched_rows + 1;
   }
}
/*
 * sql_connect($host, $user, $password, $db)
 * returns the connection ID
 */
function sql_connect($host, $user, $password, $db)
{
global $dbtype;
switch ($dbtype) {
    case "MySQL":
        $dbi=@mysql_connect($host, $user, $password);
        if (!mysql_select_db($db)) {
       //mysql_query("CREATE DATABASE $db");
       //mysql_select_db($db);
       //include("objects/installdatabase.inc");
       //checkForError(null);
       //die();
           return false;
        }
        return $dbi;
    break;;
    case "mSQL":
         $dbi=msql_connect($host);
         if (!msql_select_db($db)) {
       msql_query("CREATE DATABASE $db");
       msql_select_db($db);
       include("objects/installdatabase.inc");
       die();
         }
    return $dbi;
    break;;
    case "PostgreSQL":
         $dbi=@pg_connect("host=$host user=$user password=$password port=5432 dbname=$db");
         return $dbi;
    break;;
    case "PostgreSQL_local":
         $dbi=@pg_connect("user=$user password=$password dbname=$db");
         return $dbi;
    break;;
    case "ODBC":
         $dbi=@odbc_connect($db,$user,$password);
         return $dbi;
    break;;
    case "ODBC_Adabas":
         $dbi=@odbc_connect($host.":".$db,$user,$password);
    return $dbi;
    break;;
    case "Interbase":
         $dbi=@ibase_connect($host.":".$db,$user,$password);
         return $dbi;
    break;;
    case "Sybase":
        $dbi=@sybase_connect($host, $user, $password);
   if (!sybase_select_db($db,$dbi)) {
       sybase_query("CREATE DATABASE $db",$dbi);
           sybase_select_db($db,$dbi);
           include("objects/installdatabase.inc");
       die();
   }
   return $dbi;
    break;;
    default:
    break;;
    }
}
function sql_logout($id)
{
global $dbtype;
switch ($dbtype) {
    case "MySQL":
        $dbi=@mysql_close($id);
        return $dbi;
    break;;
    case "mSQL":
         $dbi=@msql_close($id);
         return $dbi;
    break;;
    case "PostgreSQL":
    case "PostgreSQL_local":
         $dbi=@pg_close($id);
         return $dbi;
    break;;
    case "ODBC":
    case "ODBC_Adabas":
         $dbi=@odbc_close($id);
         return $dbi;
    break;;
    case "Interbase":
         $dbi=@ibase_close($id);
         return $dbi;
    break;;
    case "Sybase":
        $dbi=@sybase_close($id);
        return $dbi;
    break;;
    default:
    break;;
    }
}

global $cached_result_index;
function sql_query($query, $id, $cache_group = 2)
{
   global $query_cache,$system_config,$cached_result_index, $sql_debug;
   global $last_sql_query ;
   $last_sql_query = $query;

   // logMessage("SQL query: ".$query, "sitelog.txt");
   if($sql_debug) echo "SQL query: ".str_replace(",",", ",$query)."<BR>";

   if(!strcasecmp(substr($query,0,6),"select") && $system_config->sql_query_caching_enabled && $query_cache->sql_cache_invalid[$cache_group]==false)
   {
      $cached_result_index=0;
      $cachedresult = $query_cache->getCachedData($query,"sql", $cache_group);
      if($cachedresult!=null && !is_array($cachedresult) && substr($cachedresult,0,14)=="database error")
      {
         // if a cached error has been found and healing timeout has been exceeded
         // set cache to null to force query again as it may be temporary error, so
         // retry after each timeout
         if(time()-$query_cache->getCachedTime($query,"sql", $cache_group) > $system_config->sql_query_caching_healing_timeout)
         {
            $cachedresult=null;
            if($sql_debug) echo("<BR>healing timeout on cached error result, rerunning query...");
            $query_cache->deleteCachedData($query,"sql", $cache_group);
         }
         else
         {
            reportError("cached ".$cachedresult);
         }
      }

      //check if query is not in cache (or an error retry)
      if($cachedresult==null )
      {
         global $dbconn;
         if($dbconn==null) connectToDatabase();
         // if($dbconn==null) return false;
	 # Should check if it is still NULL and return if so.
         $id=$dbconn;
         $uncachedresult = uncached_sql_query($query, $id);
         if(!$uncachedresult)
         {
            $query_cache->cacheData($query,"database error: ".mysql_errno()." - ".mysql_error(),"sql", $cache_group);
            reportError("database error : ".mysql_errno()." - ".mysql_error());
         }
         else
         {
            while ($record = sql_fetch_array($uncachedresult) )
            {
               $cachedresult[] = $record;
            }
            if($cachedresult==null) $cachedresult="EMPTY";
            $query_cache->cacheData($query,$cachedresult,"sql", $cache_group);
         }
      }
      return $cachedresult;
   }
   else // an insert or update (or uncached select) is occuring so invalidate cache
   {
      global $dbconn;

      if($dbconn==null) connectToDatabase();
      # Should check if it is still NULL and return if so.
      $id=$dbconn;
      if($system_config->sql_query_caching_enabled) $query_cache->invalidateSQLCache($cache_group);
      return uncached_sql_query($query, $id);
   }
}

/*
 * sql_query($query, $id)
 * executes an SQL statement, returns a result identifier
 */
function uncached_sql_query($query, $id)
{
global $dbtype;
global $sql_debug;
//if($sql_debug) echo "SQL query: ".str_replace(",",", ",$query)."<BR>";
switch ($dbtype) {
    case "MySQL":
        $res=@mysql_query($query, $id);
        return $res;
    break;;
    case "mSQL":
        $res=@msql_query($query, $id);
        return $res;
    break;;
    case "PostgreSQL":
    case "PostgreSQL_local":
   $res=pg_exec($id,$query);
   $result_set = new ResultSet;
   $result_set->set_result( $res );
   $result_set->set_total_rows( sql_num_rows( $result_set ) );
   $result_set->set_fetched_rows( 0 );
        return $result_set;
    break;;
    case "ODBC":
    case "ODBC_Adabas":
        $res=@odbc_exec($id,$query);
        return $res;
    break;;
    case "Interbase":
        $res=@ibase_query($id,$query);
        return $res;
    break;;
    case "Sybase":
        $res=@sybase_query($query, $id);
        return $res;
    break;;
    default:
    break;;
    }
}
/*
 * sql_num_rows($res)
 * given a result identifier, returns the number of affected rows
 */
function sql_num_rows($res)
{
if($res=="EMPTY") return 0;
if(gettype($res)=="array") return count($res);
global $dbtype;
switch ($dbtype) {
    case "MySQL":
        $rows=mysql_num_rows($res);
        return $rows;
    break;;
    case "mSQL":
        $rows=msql_num_rows($res);
        return $rows;
    break;;

    case "PostgreSQL":
    case "PostgreSQL_local":
        $rows=pg_numrows( $res->get_result() );
        return $rows;
    break;;
    case "ODBC":
    case "ODBC_Adabas":
        $rows=odbc_num_rows($res);
        return $rows;
    break;;
    case "Interbase":
   echo "<BR>Error! PHP dosen't support ibase_numrows!<BR>";
        return $rows;
    break;;
    case "Sybase":
        $rows=sybase_num_rows($res);
        return $rows;
    break;;
    default:
    break;;
    }
}

/*
 * sql_fetch_row(&$res,$row)
 * given a result identifier, returns an array with the resulting row
 * Needs also a row number for compatibility with PostgreSQL
 */
function sql_fetch_row(&$res, $nr=0)
{
   if($res=="EMPTY") return null;
   if(gettype($res)=="array")
   {
      global $cached_result_index;
      $cached_result_index++;
      if(isset($res[$cached_result_index-1]))
      {
         return $res[$cached_result_index-1];
      }
      else return null;
   }
   global $dbtype;
   switch ($dbtype) {
      case "MySQL":
         $row = mysql_fetch_row($res);
         return $row;
      break;;
      case "mSQL":
         $row = msql_fetch_row($res);
         return $row;
      break;;
      case "PostgreSQL":
      case "PostgreSQL_local":
      if ( $res->get_total_rows() > $res->get_fetched_rows() ) {
         $row = pg_fetch_row($res->get_result(), $res->get_fetched_rows() );
         $res->increment_fetched_rows();
         return $row;
      } else {
         return false;
      }
      break;;
      case "ODBC":
      case "ODBC_Adabas":
         $row = array();
         $cols = odbc_fetch_into($res, $nr, $row);
         return $row;
      break;;
      case "Interbase":
         $row = ibase_fetch_row($res);
         return $row;
      break;;
      case "Sybase":
         $row = sybase_fetch_row($res);
         return $row;
      break;;
      default:
      break;;
   }
}

/*
 * sql_fetch_array($res,$row)
 * given a result identifier, returns an associative array
 * with the resulting row using field names as keys.
 * Needs also a row number for compatibility with PostgreSQL.
 */
function sql_fetch_array(&$res, $nr=0)
{
global $dbtype;
switch ($dbtype)
    {
    case "MySQL":
        $row = array();
        $row = mysql_fetch_array($res);
        return $row;
    break;;
    case "mSQL":
        $row = array();
        $row = msql_fetch_array($res);
        return $row;
    break;;
    case "PostgreSQL":
    case "PostgreSQL_local":
   if( $res->get_total_rows() > $res->get_fetched_rows() ) {
      $row = array();
      $row = pg_fetch_array($res->get_result(), $res->get_fetched_rows() );
      $res->increment_fetched_rows();
      return $row;
   } else {
      return false;
   }
    break;;
/*
 * ODBC doesn't have a native _fetch_array(), so we have to
 * use a trick. Beware: this might cause HUGE loads!
 */
    case "ODBC":
        $row = array();
        $result = array();
        $result = odbc_fetch_row($res, $nr);
   $nf = odbc_num_fields($res); /* Field numbering starts at 1 */
        for($count=1; $count < $nf+1; $count++)
   {
            $field_name = odbc_field_name($res, $count);
            $field_value = odbc_result($res, $field_name);
            $row[$field_name] = $field_value;
        }
        return $row;
    break;;
    case "ODBC_Adabas":
        $row = array();
        $result = array();
        $result = odbc_fetch_row($res, $nr);
        $nf = count($result)+2; /* Field numbering starts at 1 */
   for($count=1; $count < $nf; $count++) {
       $field_name = odbc_field_name($res, $count);
       $field_value = odbc_result($res, $field_name);
       $row[$field_name] = $field_value;
   }
        return $row;
    break;;
    case "Interbase":
   $orow=ibase_fetch_object($res);
   $row=get_object_vars($orow);
        return $row;
    break;;
    case "Sybase":
        $row = sybase_fetch_array($res);
        return $row;
    break;;
    }
}
function sql_fetch_object(&$res, $nr=0)
{
global $dbtype;
switch ($dbtype)
    {
    case "MySQL":
        $row = mysql_fetch_object($res);
   if($row) return $row;
   else return false;
    break;;
    case "mSQL":
        $row = msql_fetch_object($res);
   if($row) return $row;
   else return false;
    break;;
    case "PostgreSQL":
    case "PostgreSQL_local":
   if( $res->get_total_rows() > $res->get_fetched_rows() ) {
      $row = pg_fetch_object( $res->get_result(), $res->get_fetched_rows() );
      $res->increment_fetched_rows();
      if($row) return $row;
      else return false;
   } else {
      return false;
   }
    break;;
    case "ODBC":
        $result = odbc_fetch_row($res, $nr);
   if(!$result) return false;
   $nf = odbc_num_fields($res); /* Field numbering starts at 1 */
        for($count=1; $count < $nf+1; $count++)
   {
            $field_name = odbc_field_name($res, $count);
            $field_value = odbc_result($res, $field_name);
            $row->$field_name = $field_value;
        }
        return $row;
    break;;
    case "ODBC_Adabas":
        $result = odbc_fetch_row($res, $nr);
   if(!$result) return false;
        $nf = count($result)+2; /* Field numbering starts at 1 */
   for($count=1; $count < $nf; $count++) {
       $field_name = odbc_field_name($res, $count);
       $field_value = odbc_result($res, $field_name);
       $row->$field_name = $field_value;
   }
        return $row;
    break;;
    case "Interbase":
        $orow = ibase_fetch_object($res);
   if($orow)
   {
       $arow=get_object_vars($orow);
       while(list($name,$key)=each($arow))
       {
      $name=strtolower($name);
      $row->$name=$key;
       }
           return $row;
   }else return false;
    break;;
    case "Sybase":
        $row = sybase_fetch_object($res);
        return $row;
    break;;
    }
}
/*** Function Free Result for function free the memory ***/
function sql_free_result($res) {
global $dbtype;
switch ($dbtype) {
    case "MySQL":
        if ($res == null) {
            //echo("Attempted to call mysql_free_result with NULL <BR>");
            return true;
        }
        $row = mysql_free_result($res);
        return $row;
    break;;
      case "mSQL":
        $row = msql_free_result($res);
        return $row;
    break;;

       case "PostgreSQL":
    case "PostgreSQL_local":
        $rows=pg_FreeResult( $res->get_result() );
        return $rows;
    break;;
    case "ODBC":
    case "ODBC_Adabas":
        $rows=odbc_free_result($res);
        return $rows;
    break;;
    case "Interbase":
   echo "<BR>Error! PHP dosen't support ibase_numrows!<BR>";
        return $rows;
    break;;
    case "Sybase":
        $rows=sybase_free_result($res);
        return $rows;
    break;;
   }
}
?>
