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
//global $admin_table_width

if (!isset($admin_table_width)) $admin_table_width = "800";

if (isset($editor_session) && $editor_session->isSessionOpen()) {
    $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
    $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

    //$home_url = $system_config->site_url;
    $home_url = "index.php";
    $logout_url = '<a href="'.$http_mode.'://'.$system_config->site_url.'/admin.php?logout=true">logout</a>';
    $logout_url .= ' <a href="'.$http_mode.'://'.$system_config->site_url.'/admin.php?logout=true&silent=true&loginpage=admin.php"><img src="graphics/hide.gif" alt="blank page on logout"></a>';
} else {
    $logout_url = "";
    $home_url = "";
    if (isset($OSCAILT_SCRIPT) && $OSCAILT_SCRIPT == "admin.php") $home_url = $OSCAILT_SCRIPT;
}
?>

<table cellpadding=0 cellspacing=0 width=<?=$admin_table_width?> height=100% border=0 align=center>

<!-- header section begins-->
<tr>
<td valign=top width=800 height=12 colspan=3 background="graphics/install/topoverbg.jpg"></td>
</tr>

<tr>
<td valign=top width=18 height=21 background="graphics/install/topleftbg.jpg"></td>
<td valign=top  height=21 background="graphics/install/topbarbg.jpg" class="docHeader">
<table width=100%><tr><td width=80% align=left>&nbsp; <a href="<?=$home_url?>"><img src="graphics/home.gif" alt="home"></a> &nbsp; <?=$pageTitle?> </td><td align=right><?=$logout_url?></td></tr></table>
</td>
<td valign=top width=19 height=21 background="graphics/install/toprightbg.jpg"></td>
</tr>

<tr>
<td valign=top colspan=3 background="graphics/install/topbreakbg.jpg"></td>
</tr>

<tr> 
<td valign=top width=18 background="graphics/install/leftbg.jpg"></td>
   <!-- header section ends-->

   <!-- document section begins-->
<td valign=top height=100%>

<table cellspacing=5 width=100% height=100% border=0> 
<tr>
<td height=100% width=100% valign=top>
<?php
echo $PAGE_CONTENTS;
?>
</td>
</tr>
</table>

</td>

<!-- document section ends -->

   <!-- footer section begins -->
<td valign=top width=19 background="graphics/install/rightbg.jpg"></td>
</tr>

<tr>
<td valign=top colspan=3 background="graphics/install/bottombreakbg.jpg"></td> 
</tr>

<tr>
<td valign=top width=18 height=15 background="graphics/install/bottomleftbg.jpg"></td>
<td valign=top  height=15 background="graphics/install/bottombarbg.jpg" class="docFooter">
<b> &nbsp; </b>
</td>
<td valign=top width=19 height=15 background="graphics/install/bottomrightbg.jpg"></td>
</tr>

<tr> 
<td valign=top width=800 height=28 colspan=3 background="graphics/install/bottomunderbg.jpg"></td>
</tr>
</table>
