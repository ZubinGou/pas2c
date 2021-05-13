import os
import subprocess
import sys
from sys import argv, exit
from PyQt5.QtWidgets import QApplication, QMainWindow, QMessageBox, QFileDialog, QPlainTextEdit
from PyQt5.QtGui import QFont, QFocusEvent, QTextCursor
from PyQt5.QtCore import QObject, QEvent
from mainwindow_ui import Ui_MainWindow
from math import e, acos, asin, atan, sqrt, factorial, log10, pi, e, sin, cos, tan
from qdarkstyle import load_stylesheet
from codeeditor import PascalHighlighter, CHighlighter

OUTPUT_DIR = 'output'

class Filter(QObject):
    def eventFilter(self, widget, event):
        # FocusOut event
        if event.type() == QEvent.FocusOut:
            # do custom stuff

            widget.setReadOnly(True)
            return False
        else:
            # we don't care about other events
            return False

class Console(QPlainTextEdit):
    def focusOutEvent(self, e: QFocusEvent) -> None:
        if self.isReadOnly() == True:
            return super().focusOutEvent(e)
        print('focus out')
        exe_path = os.path.join(OUTPUT_DIR, "tmp") if os.path.exists(
            os.path.join(OUTPUT_DIR, "tmp")) else os.path.join(OUTPUT_DIR, "tmp.exe")
        p = subprocess.Popen(exe_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, encoding='ascii')  # 2>&1
        out = p.communicate(input=self.toPlainText())[0]
        print(out)
        p.wait()
        self.setPlainText(out)
        # os.remove(os.path.join(OUTPUT_DIR, 'tmp.c'))
        try:
            os.remove(os.path.join(OUTPUT_DIR, 'tmp'))
        except:
            os.remove(os.path.join(OUTPUT_DIR, 'tmp.exe'))
        self.setReadOnly(True)
        return super().focusOutEvent(e)

class Pas2CEditor(QMainWindow):
    def __init__(self):
        super(Pas2CEditor, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.s_all.setStretchFactor(0, 2)
        self.ui.s_all.setStretchFactor(1, 1)
        self.ui.pt_c.setReadOnly(True)
        self.ui.pt_console.setReadOnly(True)
        self.highlight_pas = PascalHighlighter(self.ui.pt_pas.document())
        self.highlight_c = CHighlighter(self.ui.pt_c.document())
        self.setupActions()

    def setupActions(self):
        self.ui.actionNew.triggered.connect(self.newAction)
        self.ui.actionOpen.triggered.connect(self.openAction)
        self.ui.actionSave.triggered.connect(self.saveAction)
        self.ui.actionCompile.triggered.connect(self.compileAction)
        self.ui.actionRun.triggered.connect(self.runAction)

    def newAction(self):
        self.ui.pt_pas.setPlainText('')
        self.ui.pt_c.setPlainText('')
        self.restoreEditor()

    def openAction(self):
        filename = QFileDialog().getOpenFileName(
            self.ui.mainwidget, "Open File")[0]
        if os.path.exists(filename) and self.ui.pt_pas.document().isModified():
            answer = QMessageBox.question(self.ui, "Modified Code",
                                          """<b>The current code is modified</b>
                   <p>What do you want to do?</p>
                """,
                                          QMessageBox.Discard | QMessageBox.Cancel,
                                          QMessageBox.Cancel)
            if answer == QMessageBox.Cancel:
                return
        if os.path.exists(filename):
            text = open(filename, encoding='utf-8').read()
            # if len(text.split('\n')) < 30:
            #     text += '\n' * (30-len(text.split('\n')))
            self.ui.pt_pas.setPlainText(text)
            self.restoreEditor()

    def compileAction(self):
        # self.ui.pt_console.setReadOnly(False)
        # self.ui.pt_c.setReadOnly(False)
        with open(os.path.join("tmp.pas"), "w", encoding='utf-8') as f:
            f.write(self.ui.pt_pas.toPlainText())
        cmd = [os.path.join(".", "p2c"), "tmp.pas", "-l",
               "-o", os.path.join(OUTPUT_DIR, 'tmp.c'), "-g", "grammar.json"]
        print(" ".join(cmd))
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        for c in iter(lambda: p.stdout.read(1), b''):
            self.ui.pt_console.moveCursor(QTextCursor.End)
            self.ui.pt_console.insertPlainText(c.decode('utf-8'))
        p.wait()
        with open(os.path.join(OUTPUT_DIR, 'tmp.c'), 'r', encoding='utf-8') as f:
            self.ui.pt_c.setPlainText(f.read())
        os.remove('tmp.pas')
        os.remove('log.txt')
        self.ui.actionRun.setEnabled(True)

    def saveAction(self):
        filename = QFileDialog().getSaveFileName(self.ui.mainwidget, 'Save file',
                                                 filter='*.pas', initialFilter='*.pas')[0]
        if os.path.exists(filename):
            with open(filename, 'w') as f:
                text = self.ui.pt_pas.toPlainText()
                f.write(text)

    def runAction(self):
        self.ui.pt_console.clear()
        cmd = ["gcc", "-O2", os.path.join(OUTPUT_DIR, 'tmp.c'),
               "-o", os.path.join(OUTPUT_DIR, 'tmp')]
        print(" ".join(cmd))
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        for c in iter(lambda: p.stdout.read(1), b''):
            print(c)
            self.ui.pt_console.appendPlainText(c)
        self.ui.pt_console.clear()
        p.wait()
        self.ui.pt_console.setReadOnly(False)
        # print(self.ui.pt_console.isReadOnly())

    def restoreEditor(self):
        # Enable/Disable actions
        self.ui.actionCompile.setEnabled(True)
        self.ui.actionRun.setEnabled(False)
        # Re-enable editor
        self.ui.pt_pas.setFocus()
        self.ui.pt_c.clear()
        self.ui.pt_console.clear()


if __name__ == "__main__":
    app = QApplication(argv)
    app.setStyleSheet(load_stylesheet(qt_api='pyqt5'))
    font = QFont()
    font.setFamily("Consolas")
    app.setFont(font)

    editor = Pas2CEditor()
    editor.show()
    exit(app.exec_())
