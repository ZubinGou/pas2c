import os
import subprocess
from sys import argv, exit
from PyQt5.QtWidgets import QApplication, QMainWindow, QMessageBox, QFileDialog
from PyQt5.QtGui import QFont
from mainwindow_ui import Ui_MainWindow
from math import e, acos, asin, atan, sqrt, factorial, log10, pi, e, sin, cos, tan
from qdarkstyle import load_stylesheet
from codeeditor import PascalHighlighter, CHighlighter

class Pas2CEditor(QMainWindow):
    def __init__(self):
        super(Pas2CEditor, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.s_all.setStretchFactor(0, 2)
        self.ui.s_all.setStretchFactor(1, 1)
        self.ui.pt_c.setReadOnly()
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
        # self.stopAction()
        self.ui.pt_pas.setPlainText('')
        self.ui.pt_c.setPlainText('')
        self.restoreEditor()
    
    def openAction(self):
        # self.stopAction()
        filename = QFileDialog().getOpenFileName(self.ui.mainwidget, "Open File")[0]
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
        # TODO
        with open('tmp.c', 'w', encoding='utf-8') as f:
            f.write(self.ui.pt_c.toPlainText())
        self.ui.actionRun.setEnabled(True)
    
    def saveAction(self):
        # self.stopAction()
        filename = QFileDialog().getSaveFileName(self.ui.mainwidget, 'Save file', filter='*.pas', initialFilter='*.pas')[0]
        if os.path.exists(filename):
            with open(filename,'w') as f:
                text=self.ui.pt_pas.toPlainText()
                f.write(text)

    def runAction(self):
        cmd = ["gcc", "-O2", 'tmp.c', "-o", 'tmp'] 
        p = subprocess.Popen(cmd)
        p.wait()
        subprocess.call(os.path.join(".", "tmp"))
        os.remove('tmp.c')
        os.remove('tmp')

    def restoreEditor(self):
        # Enable/Disable actions
        self.ui.actionCompile.setEnabled(True)
        self.ui.actionRun.setEnabled(False)
        # Re-enable editor
        self.ui.pt_pas.setFocus()

if __name__ == "__main__":
    app = QApplication(argv)
    app.setStyleSheet(load_stylesheet(qt_api='pyqt5'))
    font = QFont()
    font.setFamily("Consolas")
    app.setFont(font)

    editor = Pas2CEditor()
    editor.show()
    exit(app.exec_())
