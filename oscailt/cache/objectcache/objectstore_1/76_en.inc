

<!-- <banner-bar obj id 76> -->
<TABLE class='<?php echo $page_style; ?>'>
<TR class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbar", 1)))
          {
          ?>
             <TD style='width: 1%; padding: 0; vertical-align: middle;'>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=bar&amp;target_id=76&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Horizontal Bar [banner-bar] Configuration" alt="Edit Horizontal Bar [banner-bar] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             </TD>
          <?
          }
          ?>
       <TD class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcodebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=codebox&amp;target_id=2207&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Code Box [Donate Online] Configuration" alt="Edit Code Box [Donate Online] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Donate Online obj id 2207> -->
<center>
<a href="https://co.clickandpledge.com/sp/d1/default.aspx?wid=48434" target="_blank"> <IMG src="http://images.clickandpledge.com/flair/buttons/97x84/CP_EN_OR_A_001.gif" border="0" alt="Online donation system by ClickandPledge" title ="Online donation system by ClickandPledge"></a>
<!---<hr>
<br><b>Donations in Euro</b><br />
<form action="https://www.paypal.com/cgi-bin/webscr" method="post">
<input type="hidden" name="cmd" value="_donations">
<input type="hidden" name="business" value="donation@imemc.org">
<input type="hidden" name="item_name" value="IMEMC News">
<input type="hidden" name="page_style" value="PayPal">
<input type="hidden" name="no_shipping" value="1">
<input type="hidden" name="return" value="http://www.imemc.org">
<input type="hidden" name="cn" value="Please leave us a note">
<input type="hidden" name="currency_code" value="EUR">
<input type="hidden" name="tax" value="0">
<input type="hidden" name="bn" value="PP-DonationsBF">
<input type="image" src="/attachments/sites/site_1/paypaleuro.gif" border="0" name="submit" alt="Make payments with PayPal - it's fast, free and secure!">
</form>
<b>Donations in USD</b><br />
 <form action="https://www.paypal.com/cgi-bin/webscr" method="post">
<input type="hidden" name="cmd" value="_donations">
<input type="hidden" name="business" value="donation@imemc.org">
<input type="hidden" name="item_name" value="IMEMC News">
<input type="hidden" name="no_shipping" value="0">
<input type="hidden" name="no_note" value="1">
<input type="hidden" name="currency_code" value="USD">
<input type="hidden" name="tax" value="0">
<input type="hidden" name="lc" value="US">
<input type="hidden" name="bn" value="PP-DonationsBF">
<input type="image" src="/attachments/sites/site_1/paypaldollar.gif" border="0" name="submit" alt="Make payments with PayPal - it's fast, free and secure!">
</form>
<hr>--->
</center>
<!-- </Donate Online obj id 2207> -->

</TD>
<TD class='banner-right'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpicturebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=picturebox&amp;target_id=1932&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Picture Box [site-banner] Configuration" alt="Edit Picture Box [site-banner] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <site-banner obj id 1932> -->
<a href='<?php echo $pseudo_directory_offset; ?>index.php' class='banner-right'><img class='banner-right-thumbnail' src='<?php echo getResizedImageURL("graphics/newheader_1.jpg", 0, 0, false, 0); ?>' alt='International Middle East Media Center' title='International Middle East Media Center' /></a>
<!-- </site-banner obj id 1932> -->

</TD>
</TR>
</TABLE>

<!-- </banner-bar obj id 76> -->

