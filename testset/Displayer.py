
import collections
import os
import json
import logging
logging.basicConfig(level = logging.DEBUG,format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s')

from ctypes import c_int,c_ulong, c_float, c_double
import time
from math import sin, cos, pi
import random
randint = random.randint
choice = random.choice

import qrcode
import pyglet

#pyglet.options['debug_gl'] = False
from pyglet.gl import *
from pyglet.window import mouse, key
from pyglet.window import FPSDisplay
from pyglet import clock

def randchar(num):
  return "".join(chr(randint(65, 90)) for i in range(num))
  #return "".join(chr(randint(33, 126)) for i in range(num))




display = pyglet.canvas.get_display()

class Displayer(pyglet.window.Window):
  COLOR_INDEX = (
    (255,   0,   0),    # Red
    (  34, 139,   34),  # Green
    (  0,   0, 205),    # Blue
    (360, 32,   240),   # Purple
    (138,   18, 137),   # Pink
    (  0, 0, 0),        # black
  )
  ROUND_TIME_CONST = (
    1,      # 10s
    0.5,    # 10s
    0.33,   # 10s
    0.2,    # 10s
    0.15,   # 10s
    0.1,    # 10s
  )
  SEC_PER_ROUND = 10

  def __init__(self, *a, **ka):
    ka.update(width=1024, height=768, caption="Embedded Test", resizable=False, vsync=False, screen=display.get_screens()[0])
    super().__init__(*a, **ka)
    self.fps_display = FPSDisplay(self)
    self.set_minimum_size(1024, 768)

    glPolygonMode(GL_FRONT, GL_FILL)
    glPolygonMode(GL_BACK, GL_FILL)
    glLineWidth(3)
    glDisable(GL_DEPTH_TEST)
    glDisable(GL_LIGHTING)

    glClearColor(1, 1, 1, 1)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glFlush()

    self.hint_doc = pyglet.text.decode_html(
      '<h1>Server Starting...<h1><br><h2>Press &lt;SPACE&gt; to start test</h2><h3>IP:PORT->xxx.xxx.xxx.xxx:xxxx</h3><br>Current Clients:[]',
    )
    self.hint_label = pyglet.text.layout.TextLayout(self.hint_doc, self.width, self.height, multiline=True, wrap_lines=True)
    '''
      font_name='Times New Roman',
      font_size=15,
      x=self.width//2, y=self.height//2,
      multiline=True, width=self.width,
      anchor_x='center', anchor_y='center','''
    self.pid_label = pyglet.text.Label('',
                          font_name='Times New Roman',
                          font_size=15,
                          color=(0,0,0,255),
                          x=20, y=self.height - 310)
    self.fps_label = pyglet.text.Label('',
                          font_name='Times New Roman',
                          font_size=15,
                          color=(0,0,0,255),
                          x=20, y=self.height - 330)
    self.init_all_reg()
    self.qr = None
    # clock.schedule(self.tick)
    self.a_pps = clock.Clock()

  def init_all_reg(self):
    self.vbuf = list()
    self.vp = set()
    self.qr = qrcode.QRCode(
        version=None,
        error_correction=qrcode.constants.ERROR_CORRECT_L,
        box_size=12,
        border=2
    )
    self.pic_tick = 0
    self.answer = list()
    self.tick_time = 0
    self.round = 0
    self.round_cnt = 0
    self.my_cnt = 0

  '''
num == 1：
  一个矩形, size = ([.3,.7], [.3,.7]), 全染，
  algorithm:
    随机起始点，width, height

num == 2:
  一个矩形, size = ([.3,.4], [.3,.4])， 全染
  一个三角形，size = ([.3,.4], [.3,.4])，

num == 3：
  一个矩形
  一个三角形
  一个圆形
 algorithm:


'''



  def split_view_port(self, num: int):

    def check_in_rec(s, x, y, w, h):
      for rec in s:
        if max(rec[0], x) < min(rec[0] + rec[2], x + w) and max(rec[1], y) < min(rec[1] + rec[3], y + h):
          return False
      return True

    xlist = [0]
    ylist = [0]
    unit_x = [int((1024 / 10) * i) for i in range(10)]
    unit_y = [int((768 / 10) * i) for i in range(10)]

    final_view_port_0 = [(0,unit_y[6],unit_x[3], unit_y[9])]
    final_view_port = set()

    if num == 1:
      while True:
        x = randint(5, unit_x[7] - 10)
        y = randint(5, unit_x[7] - 10)
        w = randint(unit_x[3], unit_x[7] - 20)
        h = randint(unit_x[3], unit_x[7] - 20)
        if x + w >= 1000 or y + h >= 750:
          continue
        if check_in_rec(final_view_port_0, x, y, w, h):
          return set([(x, y, x + w, y + h)])
    if num == 2:
      cnt = 0
      while len(final_view_port) < num and cnt < 500:
        cnt += 1
        x = randint(5, unit_x[7] - 10)
        y = randint(5, unit_x[7] - 10)
        w = randint(unit_x[3], unit_x[4] - 20)
        h = randint(unit_x[3], unit_x[4] - 20)
        if x + w >= 1000 or y + h >= 750:
          continue
        if check_in_rec(final_view_port_0, x, y, w, h):
          final_view_port_0.append((x, y, x + w, y + h))
          final_view_port.add((x, y, x + w, y + h))
    if num == 3:
      cnt = 0
      while len(final_view_port) < num and cnt < 500:
        cnt += 1
        x = randint(5, unit_x[7] - 10)
        y = randint(5, unit_x[7] - 10)
        w = randint(unit_x[2], unit_x[4] - 20)
        h = randint(unit_x[2], unit_x[4] - 20)
        if x + w >= 1000 or y + h >= 750:
          continue
        if check_in_rec(final_view_port_0, x, y, w, h):
          final_view_port_0.append((x, y, x + w, y + h))
          final_view_port.add((x, y, x + w, y + h))

    return final_view_port

  def make_image(self, num: int):
    self.vp = self.split_view_port(num)
    shapes = list()
    for __ in range(num):
      shape_type = randint(3, 6)
      verts = list()
      col = choice(self.COLOR_INDEX)
      sa = list()
      shapes.append(shape_type)
      if shape_type == 6:
        r = randint(150, 255) / 512
        for vi in range(360):
          a = vi * pi / 180
          verts.append(.5 + r * sin(a))#x
          verts.append(.5 + r * cos(a))#y
        self.vbuf.append(pyglet.graphics.vertex_list(360,
          ('v2f', verts),
          ('c3B', col * 360),
        ))
      else:
        begin = randint(0, 30)
        while len(sa) < shape_type:
          sa.append(begin)
          begin = (begin + 360 // shape_type) % 360
        for vi in range(shape_type):
          r = randint(150, 255) / 512
          a = sa[vi] * pi / 180
          x = .5 + r * sin(a)
          y = .5 + r * cos(a)
          verts.append(x)
          verts.append(y)
        self.vbuf.append(pyglet.graphics.vertex_list(shape_type,
          ('v2f', verts),
          ('c3B', col * shape_type),
        ))

    self.qr.clear()
    # payload = f"{self.round}-{self.round_cnt}"
    payload = "{0:04d}".format(self.my_cnt)
    self.my_cnt = self.my_cnt + 1
    # self.pid_label.text = f"No.{payload}"
    # self.fps_label.text = f"{1/self.pic_tick if self.pic_tick else 0:.1f}/{1/self.ROUND_TIME_CONST[self.round]:.1f}fps"#self.a_pps.get_fps()
    payload = f"{payload}-{randchar(10)}"
    self.qr.add_data(payload)
    print(payload)
    self.qr.make(fit=True)
    return shapes, payload

  def tick(self, dt):
    self.tick_time += dt
    rt = self.ROUND_TIME_CONST[self.round]
    if self.tick_time > rt:
      self.tick_time -= rt
      self.round_cnt += 1
      if self.round_cnt >= self.SEC_PER_ROUND / self.ROUND_TIME_CONST[self.round]:
        self.round += 1
        self.round_cnt = 0
        if self.round == len(self.ROUND_TIME_CONST):
          clock.unschedule(self.tick)
          [x.delete() for x in self.vbuf]
          self.vbuf.clear()
          self.qr = None
          self.on_draw()
          open(os.path.join(ANS_DIR, f"{time.time():.0f}.json"), "w").write(json.dumps([(collections.Counter(shape), payload) for shape, payload in self.answer]))
          return
      [x.delete() for x in self.vbuf]
      self.vbuf.clear()
      self.answer.append(self.make_image(randint(1, 3)))
      self.pic_tick = self.a_pps.tick()
      #[print(list(x.vertices)) for x in self.vbuf]
      #[print(x) for x in self.vp]

  def on_draw(self):
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0, self.width, 0, self.height, -1, 1)
    glViewport(0, 0, self.width, self.height)
    glPushMatrix()

    for vp, sp in zip(self.vp, self.vbuf):
      glViewport(vp[0], vp[1], vp[2]-vp[0], vp[3]-vp[1])
      glMatrixMode(GL_MODELVIEW)
      glLoadIdentity()
      glMatrixMode(GL_PROJECTION)
      glLoadIdentity()
      glOrtho(0, 1, 0, 1, -1, 1)
      sp.draw(GL_POLYGON)

    '''
    for vp in self.vp:
      glBegin(GL_LINE_LOOP)
      glVertex2f(vp[0], vp[1])
      glVertex2f(vp[0], vp[3])
      glVertex2f(vp[2], vp[3])
      glVertex2f(vp[2], vp[1])
      glEnd()
    '''

    if self.qr:
      glMatrixMode(GL_MODELVIEW)
      glLoadIdentity()
      glMatrixMode(GL_PROJECTION)
      glLoadIdentity()
      glOrtho(0, self.width, self.height, 0, -1, 1)
      glViewport(0, 0, self.width, self.height)

      glColor3f(1, 0,0) #Border
      glBegin(GL_LINE_STRIP)
      glVertex2d(0, 290)
      glVertex2d(290, 290)
      glVertex2d(290, 0)
      glEnd()

      qr = self.qr
      glColor3f(0,0,0)
      glTranslatef(20, 20, 0) #QRcode
      glBegin(GL_QUADS)
      for r in range(qr.modules_count):
        for c in range(qr.modules_count):
          if qr.modules[r][c]:
            glVertex2f(r*10,      c*10)
            glVertex2f(r*10,      c*10+10)
            glVertex2f(r*10+10,   c*10+10)
            glVertex2f(r*10+10,   c*10)

      glEnd()
      glPopMatrix()
      self.pid_label.draw()#Display Info
      # self.fps_label.draw()
    else:
      glPopMatrix()
      self.hint_label.draw()

    # self.fps_display.draw()
    return

  def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
    return pyglet.event.EVENT_HANDLED


  def on_mouse_press(self, x, y, button, modifiers):
    return pyglet.event.EVENT_HANDLED

  def on_mouse_release(self, x, y, button, modifiers):
    return pyglet.event.EVENT_HANDLED



  def on_mouse_motion(self, x, y, dx, dy):
    return pyglet.event.EVENT_HANDLED


  def on_mouse_scroll(self, x, y, scroll_x, scroll_y):
    return pyglet.event.EVENT_HANDLED


  def on_resize(self, width, height):
    self.view_port = (0, 0, width, height)

    return pyglet.event.EVENT_HANDLED

  def on_key_release(self, symbol, modifiers):
    super().on_key_press(symbol, modifiers)
    if symbol == key.SPACE:
      if self.qr is None:
        self.init_all_reg()
        clock.schedule(self.tick)
    return

  def on_key_press(self, symbol, modifiers):
    super().on_key_press(symbol, modifiers)






if __name__ == "__main__":
  ANS_DIR = "./ans/"

  if not os.path.isdir(ANS_DIR):
    os.mkdir(ANS_DIR)
  main_window = Displayer()

  pyglet.app.run()
