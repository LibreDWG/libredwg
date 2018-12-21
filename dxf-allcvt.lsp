;*****************************************************************************/
;*  LibreDWG - free implementation of the DWG file format                    */
;*                                                                           */
;*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
;*                                                                           */
;*  This library is free software, licensed under the terms of the GNU       */
;*  General Public License as published by the Free Software Foundation,     */
;*  either version 3 of the License, or (at your option) any later version.  */
;*  You should have received a copy of the GNU General Public License        */
;*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
;*****************************************************************************/
;
;;; dxf-allcvt.lsp: DXFIN all generated dxf files
;;; first load a dxf from the root dir manually to set the DWGPREFIX path
;;; written by Reini Urban

;;; Usage: ./dxf-allcvt.sh
;;;        OPEN libredwg/Drawing_2000.dxf
;;;        (load (strcat (getvar "DWGPREFIX") "dxf-allcvt.lsp"))
;;;        DXF-ALLCVT
(setvar "FILEDIA" 1)

(defun C:dxf-allcvt ()
  (setvar "FILEDIA" 0)
  (setvar "CMDDIA" 1)
  (setvar "CMDECHO" 1)
  (command) ; break an active command
  (setq path (getvar "DWGPREFIX")) ; should end with libredwg
  (setq all (vl-directory-files path "*.dxf" 1)) ; globbing does not work for me
  (setq files (apply 'append
                     (mapcar '(lambda (fn)
                                (if (wcmatch fn "*.dxf")
                                    (list (strcat path fn)) nil))
                             all)))
  (foreach dxf files
    (print dxf)
    (command "._DXFIN" dxf)
    (getstring "Enter for next DXF :") (command)
    ;(command "._CLOSE")
    )
)
