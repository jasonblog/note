package ch04;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.util.concurrent.TimeUnit;

import com.xuggle.mediatool.ToolFactory;
import com.xuggle.mediatool.IMediaWriter;
import com.xuggle.xuggler.IRational;


public class Ch04_2_7RecordScreenToFile
{
    private static IRational FRAME_RATE = IRational.make(3, 1);
    private static final int SECONDS_TO_RUN_FOR = 15;//錄15秒


    public static void main(String[] args)
    {
        try {
            final String outFile;

            if (args.length > 0) {
                outFile = args[0];
            } else {
                outFile = "c:\\opencv3.1\\samples\\RecordScreenToFile.mp4";
            }

            //使用Java Robot去抓螢幕畫面
            final Robot robot = new Robot();
            final Toolkit toolkit = Toolkit.getDefaultToolkit();
            final Rectangle screenBounds = new Rectangle(toolkit.getScreenSize());

            //宣告IMediaWriter以便寫入檔案.
            final IMediaWriter writer = ToolFactory.makeWriter(outFile);

            //使用video stream API
            writer.addVideoStream(0, 0,
                                  FRAME_RATE,
                                  screenBounds.width, screenBounds.height);

            // loop
            long startTime = System.nanoTime();

            for (int index = 0; index < SECONDS_TO_RUN_FOR * FRAME_RATE.getDouble();
                 index++) {
                //抓整個螢幕畫面
                BufferedImage screen = robot.createScreenCapture(screenBounds);

                // 格式轉換
                BufferedImage bgrScreen = convertToType(screen,
                                                        BufferedImage.TYPE_3BYTE_BGR);

                // 編碼
                writer.encodeVideo(0, bgrScreen,
                                   System.nanoTime() - startTime, TimeUnit.NANOSECONDS);

                System.out.println("encoded image: " + index);

                // 間格
                Thread.sleep((long)(1000 / FRAME_RATE.getDouble()));

            }

            //結束
            writer.close();
        } catch (Throwable e) {
            System.err.println("an error occurred: " + e.getMessage());
        }
    }

    public static BufferedImage convertToType(BufferedImage sourceImage,
            int targetType)
    {
        BufferedImage image;


        if (sourceImage.getType() == targetType) {
            image = sourceImage;
        }

        else {
            image = new BufferedImage(sourceImage.getWidth(),
                                      sourceImage.getHeight(), targetType);
            image.getGraphics().drawImage(sourceImage, 0, 0, null);
        }

        return image;
    }

}
