<?php

/**
 * @file
 * Default theme implementation to display a member.
 */

?>
<div id="member-<?php print $member->member_id; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php if (!$page): ?>
    <h2><a href="<?php print $url; ?>"><?php print $label; ?></a></h2>
  <?php endif; ?>

  <?php if ($username): ?>
    <div class="pseudo-field member-username">
      <span class="label">Username: </span><span class="item"><?php print $username; ?></span>
    </div>
  <?php endif; ?>

  <?php if ($uuid): ?>
    <div class="pseudo-field member-uuid">
      <span class="label">UUID: </span><span class="item"><?php print $uuid; ?></span>
    </div>
  <?php endif; ?>

  <div class="content"<?php print $content_attributes; ?>>
    <?php print render($content); ?>
  </div>

</div>
