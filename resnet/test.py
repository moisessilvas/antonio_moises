import tensorflow as tf

from tensorflow.keras import datasets, layers, models
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import matplotlib.pyplot as plt
import cv2 as cv

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

print(traingen)

model = models.Sequential()
model.add(layers.Input(shape=(224, 224, 1)))
model.add(layers.Conv2D(64, (7, 7), (1, 1), kernel_initializer='ones', padding='same'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Conv2D(64, (3, 3), (1, 1), kernel_initializer='ones', padding='same'))
model.add(layers.Flatten())
model.add(layers.Dense(1), kernel_initializer='ones')

model.compile(loss="binary_crossentropy", optimizer='adam')
model.fit(traingen, epochs=1, steps_per_epoch=1)

model.summary()

model.save("model.h5")
out = model.predict(traingen)
print(out)
