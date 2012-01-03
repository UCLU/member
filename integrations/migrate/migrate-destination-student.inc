<?php
/**
 * @file
 * MigrateDestinationStudent class.
 */
class MigrateDestinationStudent extends MigrateDestinationEntity {

  public function __construct($bundle, array $options = array()) {
    parent::__construct('student', $bundle, $options);
  }

  static public function getKeySchema() {
    return array(
      'student_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'ID of destination student',
      ),
    );
  }

  public function fields() {
    $fields = array();
    $fields['student_id'] = t('Student: existing student ID');
    $fields['uid'] = t('Student: Drupal user ID');
    $fields['uuid'] = t('Student: universally unique identifier');
    $fields['created'] = t('Student created timestamp');
    $fields['changed'] = t('Student: last modified timestamp');

    // Add in anything provided by handlers
    $fields += migrate_handler_invoke_all('Entity', 'fields', $this->entityType, $this->bundle);
    $fields += migrate_handler_invoke_all('Student', 'fields', $this->entityType, $this->bundle);

    return $fields;
  }


  /**
   * Delete a batch of students at once.
   *
   * @param $student_ids
   *  Array of student IDs to be deleted.
   */
  public function bulkRollback(array $student_ids) {
    migrate_instrument_start('student_delete_multiple');
    $this->prepareRollback($student_ids);
    student_delete_multiple($student_ids);
    $this->completeRollback($student_ids);
    migrate_instrument_stop('student_delete_multiple');
  }

  /**
   * Import a single student.
   *
   * @param $student
   *  Student object to build. Prefilled with any fields mapped in the Migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *  Array of key fields (student_id only in this case) of the student that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $student, stdClass $row) {
    // Updating previously-migrated content?
    $migration = Migration::currentMigration();
    if (isset($row->migrate_map_destid1)) {
      // Make sure is_new is off
      $student->is_new = FALSE;
      if (isset($student->student_id)) {
        if ($student->student_id != $row->migrate_map_destid1) {
          throw new MigrateException(t("Incoming student_id !student_id and map destination student_id !destid1 don't match",
            array('!student_id' => $student->student_id, '!destid1' => $row->migrate_map_destid1)));
        }
      }
      else {
        $student->student_id = $row->migrate_map_destid1;
      }
    }
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      if (!isset($student->student_id)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but no destination student_id provided'));
      }
      $old_student = student_load($student->student_id);
      if (!isset($student->created)) {
        $student->created = $old_student->created;
      }
      if (!isset($student->uid)) {
        $student->uid = $old_student->uid;
      }
      if (!isset($student->uuid)) {
        $student->uuid = $old_student->uuid;
      }
    }

    // Set some required properties.
    // Set type before invoking prepare handlers - they may take type-dependent actions.
    $student->type = $this->bundle;

    if ($migration->getSystemOfRecord() == Migration::SOURCE) {
      if (!isset($student->language)) {
        $student->language = $this->language;
      }
      // Apply defaults, allow standard student prepare hooks to fire.
      // student_object_prepare() will blow these away, so save them here and
      // stuff them in later if need be.
      if (isset($student->created)) {
        $created = MigrationBase::timestamp($student->created);
      }
      else {
        // To keep student_object_prepare() from choking
        $student->created = REQUEST_TIME;
      }
      if (isset($student->changed)) {
        $changed = MigrationBase::timestamp($student->changed);
      }
      if (isset($student->uid)) {
        $uid = $student->uid;
      }
      student_object_prepare($student);
      if (isset($created)) {
        $student->created = $created;
      }
      // No point to resetting $student->changed here, student_save() will overwrite it
      if (isset($uid)) {
        $student->uid = $uid;
      }
    }

    // Invoke migration prepare handlers
    $this->prepare($student, $row);

    // Trying to update an existing student
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      // Incoming data overrides existing data, so only copy non-existent fields
      foreach ($old_student as $field => $value) {
        // An explicit NULL in the source data means to wipe to old value (i.e.,
        // don't copy it over from $old_student)
        if (property_exists($student, $field) && $student->$field === NULL) {
          // Ignore this field
        }
        elseif (!isset($student->$field)) {
          $student->$field = $old_student->$field;
        }
      }
    }

    if (isset($student->student_id) && !(isset($student->is_new) && $student->is_new)) {
      $updating = TRUE;
    }
    else {
      $updating = FALSE;
    }

    migrate_instrument_start('student_save');
    student_save($student);
    migrate_instrument_stop('student_save');

    if (isset($student->student_id)) {
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }

      // Unfortunately, http://drupal.org/node/722688 was not accepted, so fix
      // the changed timestamp
      if (isset($changed)) {
        db_update('student')
          ->fields(array('changed' => $changed))
          ->condition('student_id', $student->student_id)
          ->execute();
        $student->changed = $changed;
      }

      $return = array($student->student_id);
    }
    else {
      $return = FALSE;
    }

    $this->complete($student, $row);
    return $return;
  }

}