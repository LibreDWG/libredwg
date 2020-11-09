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

(defun C:dxf-allcvt (/ match files prompt erase)
  (setvar "FILEDIA" 0)
  (setvar "CMDDIA" 1)
  (setvar "CMDECHO" 1)
  (setvar "LOGFILEMODE" 1)
  (command) ; break an active command
  (setq active (getvar "DWGNAME"))
  (setq path (getvar "DWGPREFIX")) ; should end with libredwg
  (setq all (vl-directory-files path "*.dxf" 1)) ; globbing does not work for me
  (setq prompt (getstring "Optional prompt <N> : ")
        prompt (if (and prompt (or (= prompt "Y") (= prompt "y"))) 1 nil))
  (setq erase (getstring "Optional erase <N> : ")
        erase (if (and erase (or (= erase "Y") (= erase "y"))) 1 nil))
  (setq match (getstring "Optional match <*.dxf> : ")
        match (if (or (not match) (= match "")) "*.dxf" match))
  (setq files (apply 'append
                     (mapcar '(lambda (fn)
                                (if (wcmatch fn match)
                                    (list fn) nil))
                             all)))
  (foreach dxf files
    (if (or (wcmatch dxf "SALLE_DES_MACHINES_2007.dxf") ; xdata TU hang
            (wcmatch dxf "Leader_20*.dxf") ; silent crash
            (wcmatch dxf "1_*from_cadforum.cz_AC1018.dxf") ; dbobji.cpp@306 assert
            (wcmatch dxf "3_*from_cadforum.cz_AC1018.dxf") ; dbobji.cpp@306 assert
            (wcmatch dxf "kacena_from_cadforum.cz_AC1018.dxf") ; silent crash
            (wcmatch dxf "5151-019_2010.dxf")  ; silent crash
            )
        (print (strcat dxf " skipped"))
      (progn
        (if prompt
            (progn (getstring (strcat "Enter for next DXF " dxf ":"))
                   (command)))
        (command "._DXFIN" (strcat path dxf))
        (setvar "CMDECHO" 0)
        ;(close-all-but active)
        ; This produes annoying Duplicate Block warnings in the log
        (if erase
            (progn
              (command "._ERASE" "All" "")
              (command "._PURGE" "All" "*" "N")))
        (setvar "CMDECHO" 1)
        ;;;(command "._CLOSE")
        )
      )
    )
  (setvar "FILEDIA" 1)
  (setvar "LOGFILEMODE" 0)
)
