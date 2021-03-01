import tensorflow as tf

from tensorflow.keras import datasets, layers, models
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import matplotlib.pyplot as plt
import cv2 as cv

tf.keras.backend.set_floatx('float64')
'''
image = cv.imread("/home/antonio/Imagens/gray/g/Grayscale2.jpg")
gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
print(gray.shape)
#cv.imwrite("/home/antonio/Imagens/gray/g/Grayscale2.jpg", gray)
'''
datagen = ImageDataGenerator()
traingen = datagen.flow_from_directory(
    directory="/home/antonio/Imagens/gray/",
    class_mode='input',
    color_mode='grayscale',
    batch_size=1,
    target_size=(224,224))

inputs = layers.Input(shape=(224, 224, 1))

# Layer 1
x = layers.ZeroPadding2D((3,3))(inputs)
x = layers.Conv2D(64, (7, 7), (2, 2), kernel_initializer='ones', padding='valid', use_bias=False)(x)

# Layer 2
x = layers.ZeroPadding2D((1,1))(x)
x1 = layers.MaxPooling2D((3, 3), strides=(2,2), padding='valid')(x)
x = layers.ZeroPadding2D((1,1))(x1)
x = layers.Conv2D(64, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(64, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([x1,x2])
x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(64, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(64, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([added,x3])

# Layer 3

sc = layers.Conv2D(128, (1, 1), (2, 2), kernel_initializer='ones', padding='valid', use_bias=False)(added)
x = layers.ZeroPadding2D((1, 1))(added)
x = layers.Conv2D(128, (3, 3), (2, 2), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(128, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([sc,x2])
x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(128, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(128, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([added,x3])
x = layers.ZeroPadding2D((1,1))(added)

# Layer 4

sc = layers.Conv2D(256, (1, 1), (2, 2), kernel_initializer='ones', padding='valid', use_bias=False)(added)
x = layers.ZeroPadding2D((1, 1))(added)
x = layers.Conv2D(256, (3, 3), (2, 2), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(256, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([sc,x2])
x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(256, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(256, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([added,x3])
x = layers.ZeroPadding2D((1,1))(added)

# Layer 5

sc = layers.Conv2D(512, (1, 1), (2, 2), kernel_initializer='ones', padding='valid', use_bias=False)(added)
x = layers.ZeroPadding2D((1, 1))(added)
x = layers.Conv2D(512, (3, 3), (2, 2), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(512, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([sc,x2])
x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(512, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(512, (3, 3), kernel_initializer='ones', padding='valid', use_bias=False)(x)
added = layers.add([added,x3])
x = layers.ZeroPadding2D((1,1))(added)

flat = layers.Flatten()(added)
output = layers.Dense(1, kernel_initializer='ones', use_bias=False)(flat)
model = models.Model(inputs=inputs, outputs=output)

model.compile(loss="binary_crossentropy", optimizer='adam')
model.fit(traingen, epochs=1, steps_per_epoch=1)

model.summary()

model.save_weights("weights.h5")
out = model.predict(traingen)
print(out)
