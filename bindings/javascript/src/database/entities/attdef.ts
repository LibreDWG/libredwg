import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgAttachmentPoint } from './dimension'
import { DwgEntity } from './entity'
import { DwgTextBase } from './text'

export interface DwgEmbeddedMText {
  /**
   * Insertion point
   */
  insertionPoint: DwgPoint3D
  /**
   * Nominal (initial) text height
   */
  rectHeight: number
  /**
   * Reference rectangle width
   */
  rectWidth: number
  /**
   * Horizontal width of the characters that make up the mtext entity. This value will always be equal to
   * or less than the value of 'rectWidth' (read-only, ignored if supplied)
   */
  extentsWidth: number
  /**
   * Vertical height of the mtext entity (read-only, ignored if supplied)
   */
  extentsHeight: number
  /**
   * Attachment point:
   * - 1: Top left
   * - 2: Top center
   * - 3: Top right
   * - 4: Middle left
   * - 5: Middle center
   * - 6: Middle right
   * - 7: Bottom left
   * - 8: Bottom center
   * - 9: Bottom right
   */
  attachmentPoint: DwgAttachmentPoint
  /**
   * X-axis direction vector (in WCS)
   */
  direction: DwgPoint3D
}

export interface DwgAttdefEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'ATTDEF'
  /**
   * Text attributes
   */
  text: DwgTextBase

  /**
   * Prompt string
   */
  prompt: string
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
  numberOfSecondaryAttrs?: number
  /**
   * hard-pointer id of secondary attribute(s) or attribute definition(s)
   */
  secondaryAttrsHardId?: number
  /**
   * Alignment point of attribute or attribute definition.
   */
  alignmentPoint?: DwgPoint2D
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
