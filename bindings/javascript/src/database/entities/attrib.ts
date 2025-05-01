import { DwgPoint3D } from '../common'
import { DwgEmbeddedMText } from './attdef'
import { DwgEntity } from './entity'
import { DwgTextBase } from './text'

export interface DwgAttribEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'ATTRIB'
  /**
   * Text attributes
   */
  text: DwgTextBase

  /**
   * Tag string (cannot contain spaces)
   */
  tag: string
  /**
   * Attribute flags:
   * - 1: Attribute is invisible (does not appear)
   * - 2: This is a constant attribute
   * - 4: Verification is required on input of this attribute
   * - 8: Attribute is preset (no prompt during insertion)
   */
  flags: number
  /**
   * Field length (optional; default = 0) (not currently used)
   */
  fieldLength: number
  /**
   * Lock position flag. Locks the position of the attribute within the block reference.
   */
  lockPositionFlag: boolean

  /**
   * Duplicate record cloning flag (determines how to merge duplicate entries):
   * - 1: Keep existing
   */
  duplicateRecordCloningFlag: boolean
  /**
   * MText flag:
   * - 2: multiline attribute
   * - 4: constant multiline attribute definition
   */
  mtextFlag: number
  /**
   * isReallyLocked flag:
   * - 0: unlocked
   * - 1: locked
   */
  isReallyLocked: boolean
  /**
   * Number of secondary attributes or attribute definitions
   */
  numberOfSecondaryAttrs: number
  /**
   * hard-pointer id of secondary attribute(s) or attribute definition(s)
   */
  secondaryAttrsHardId: number
  /**
   * Alignment point of attribute or attribute definition.
   */
  alignmentPoint: DwgPoint3D
  /**
   * current annotation scale
   */
  annotationScale: number
  /**
   * attribute or attribute definition tag string
   */
  attrTag: string

  /**
   * MText attributes
   */
  mtext: DwgEmbeddedMText
}
