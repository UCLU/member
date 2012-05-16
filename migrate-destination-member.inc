<?php
/**
 * @file
 * MigrateDestinationMember class.
 */
class MigrateDestinationMember extends MigrateDestinationEntity {

  public function __construct($bundle, array $options = array()) {
    parent::__construct('member', $bundle, $options);
  }

  static public function getKeySchema() {
    return array(
      'member_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'ID of destination member',
      ),
      'uuid' => array(
        'description' => 'The UUID of the member (for matching against external systems).',
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
      ),
    );
  }

  public function fields() {
    $fields = array();
    $fields['member_id'] = t('Member: existing member ID');
    $fields['uid'] = t('Member: Drupal user ID, default is NULL');
    $fields['uuid'] = t('Member: universally unique identifier, default is NULL');
    $fields['status'] = t('Member: membership status, default is 1');
    $fields['created'] = t('Member: created timestamp, default is now');
    $fields['changed'] = t('Member: last modified timestamp, default is now');

    // Add in anything provided by handlers
    $fields += migrate_handler_invoke_all('Entity', 'fields', $this->entityType, $this->bundle);
    $fields += migrate_handler_invoke_all('Member', 'fields', $this->entityType, $this->bundle);

    return $fields;
  }


  /**
   * Delete a batch of members at once.
   *
   * @param $member_ids
   *  Array of member IDs to be deleted.
   */
  public function bulkRollback(array $member_ids) {
    migrate_instrument_start('member_delete_multiple');
    $this->prepareRollback($member_ids);
    member_delete_multiple($member_ids);
    $this->completeRollback($member_ids);
    migrate_instrument_stop('member_delete_multiple');
  }

  /**
   * Import a single member.
   *
   * @param $member
   *  Member object to build. Prefilled with any fields mapped in the Migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *  Array of key fields (member_id only in this case) of the member that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $member, stdClass $row) {
    // Updating previously-migrated content?
    $migration = Migration::currentMigration();
    if (isset($row->migrate_map_destid1)) {
      // Make sure is_new is off
      $member->is_new = FALSE;
      if (isset($member->member_id)) {
        if ($member->member_id != $row->migrate_map_destid1) {
          throw new MigrateException(t("Incoming member_id !member_id and map destination member_id !destid1 don't match",
            array('!member_id' => $member->member_id, '!destid1' => $row->migrate_map_destid1)));
        }
      }
      else {
        $member->member_id = $row->migrate_map_destid1;
      }
    }
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      if (!isset($member->member_id)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but no destination member_id provided'));
      }
      $old_member = member_load($member->member_id);
      if (!isset($member->created)) {
        $member->created = $old_member->created;
      }
      if (!isset($member->uid)) {
        $member->uid = $old_member->uid;
      }
      if (!isset($member->uuid)) {
        $member->uuid = $old_member->uuid;
      }
    }

    // Set some required properties.
    // Set type before invoking prepare handlers - they may take type-dependent actions.
    $member->type = $this->bundle;

    if ($migration->getSystemOfRecord() == Migration::SOURCE) {
      if (!isset($member->language)) {
        $member->language = $this->language;
      }
      // Apply defaults, allow standard member prepare hooks to fire.
      // member_object_prepare() will blow these away, so save them here and
      // stuff them in later if need be.
      if (isset($member->created)) {
        $created = MigrationBase::timestamp($member->created);
      }
      else {
        // To keep member_object_prepare() from choking
        $member->created = REQUEST_TIME;
      }
      if (isset($member->changed)) {
        $changed = MigrationBase::timestamp($member->changed);
      }
      if (isset($member->uid)) {
        $uid = $member->uid;
      }
      member_object_prepare($member);
      if (isset($created)) {
        $member->created = $created;
      }
      // No point to resetting $member->changed here, member_save() will overwrite it
      if (isset($uid)) {
        $member->uid = $uid;
      }
    }

    // Invoke migration prepare handlers
    $this->prepare($member, $row);

    // Trying to update an existing member
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      // Incoming data overrides existing data, so only copy non-existent fields
      foreach ($old_member as $field => $value) {
        // An explicit NULL in the source data means to wipe to old value (i.e.,
        // don't copy it over from $old_member)
        if (property_exists($member, $field) && $member->$field === NULL) {
          // Ignore this field
        }
        elseif (!isset($member->$field)) {
          $member->$field = $old_member->$field;
        }
      }
    }

    if (isset($member->member_id) && !(isset($member->is_new) && $member->is_new)) {
      $updating = TRUE;
    }
    else {
      $updating = FALSE;
    }

    migrate_instrument_start('member_save');
    member_save($member);
    migrate_instrument_stop('member_save');

    if (isset($member->member_id)) {
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }

      // Unfortunately, http://drupal.org/node/722688 was not accepted, so fix
      // the changed timestamp
      if (isset($changed)) {
        db_update('member')
          ->fields(array('changed' => $changed))
          ->condition('member_id', $member->member_id)
          ->execute();
        $member->changed = $changed;
      }

      $return = array($member->member_id);
    }
    else {
      $return = FALSE;
    }

    $this->complete($member, $row);
    return $return;
  }

}