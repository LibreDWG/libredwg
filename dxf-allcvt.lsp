;*****************************************************************************/
;*  LibreDWG - free implementation of the DWG file format                    */
;*                                                                           */
;*  Copyright (C) 2018, 2020 Free Software Foundation, Inc.                  */
;*                                                                           */
;*  This library is free software, licensed under the terms of the GNU       */
;*  General Public License as published by the Free Software Foundation,     */
;*  either version 3 of the License, or (at your option) any later version.  */
;*  You should have received a copy of the GNU General Public License        */
;*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
;*****************************************************************************/
;
;;; dxf-allcvt.lsp: DXFIN all generated dxf files
;;; first load a dwg or dxf from the root dir manually to set the DWGPREFIX path
;;; written by Reini Urban

;;; Usage: ./dxf-allcvt.sh
;;;        OPEN libredwg/Drawing_2000.dwg
;;;        (load (strcat (getvar "DWGPREFIX") "dxf-allcvt.lsp"))
;;;        DXF-ALLCVT *_r14.dxf
;;; Ignore all Duplicate warnings. close-all-but does not work, and purge is too noisy
(setvar "FILEDIA" 1)

(defun close-all-but (file)
  (while (/= (getvar "DWGNAME") file)
    (command "_.CLOSE" "N")
    (command)
    )
  )

(defun C:dxf-allcvt (/ match files)
  (setvar "FILEDIA" 0)
  (setvar "CMDDIA" 1)
  (setvar "CMDECHO" 1)
  (setvar "LOGFILEMODE" 1)
  (command) ; break an active command
  (setq active (getvar "DWGNAME"))
  (setq path (getvar "DWGPREFIX")) ; should end with libredwg
  (setq all (vl-directory-files path "*.dxf" 1)) ; globbing does not work for me
  (setq match (getstring "Optional match <*.dxf> : ")
        match (if (or (not match) (= match "")) "*.dxf" match))
  (setq files (apply 'append
                     (mapcar '(lambda (fn)
                                (if (wcmatch fn match)
                                    (list (strcat path fn)) nil))
                             all)))
  (foreach dxf files
    (if (or (wcmatch dxf "*/SALLE_DES_MACHINES_2007.dxf") ; xdata TU hang
            (wcmatch dxf "*/Leader_20*.dxf")) ; silent crash
        (print (strcat dxf " skipped"))
      (progn
        (command "._DXFIN" dxf)
        ;(getstring "Enter for next DXF :") (command)
        (setvar "CMDECHO" 0)
        ;(close-all-but active)
        ;(getstring "Enter for next DXF :") (command)
        ; This produes annoying Duplicate Block warnings in the log
        ;(command "._ERASE" "All" "")
        ;(command "._PURGE" "All" "*" "N")
        (setvar "CMDECHO" 1)
        ;;;(command "._CLOSE")
        )
      )
    )
  (setvar "FILEDIA" 1)
  (setvar "LOGFILEMODE" 0)
)
