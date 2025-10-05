#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>

using namespace std;

int unicols = 0, unirows = 0;
bool rotl = false, rotr = false;
const double myPI = 3.141592653589793238462643383279502884197;

struct Image {
    char ImageFileName[100];
    vector<vector<int> > ImageData;
    int cols, rows, maxGray;
    vector<char> comment;
    vector<vector<double> > filter;

    bool imageLoaded;
    bool imageModified;

    void changeBrightness(double factor) {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                ImageData[r][c] *= factor;
                if (ImageData[r][c] > maxGray)
                    ImageData[r][c] = maxGray;
            }
    }

    int loadImage(char ImageName[]) {
        ifstream FCIN(ImageName);

        if (!FCIN.is_open())
            return -1;

        char MagicNumber[5];
        char Comment[100];

        FCIN.getline(MagicNumber, 4);
        FCIN.getline(Comment, 100);
        FCIN >> cols >> rows >> maxGray;

        ImageData.clear();
        ImageData.resize(rows, vector<int>(cols, 0));

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                FCIN >> ImageData[r][c];

        if (FCIN.fail())
            return -2;

        FCIN.close();
        imageLoaded = true;
        imageModified = false;
        strcpy_s(ImageFileName, ImageName);
        return 0;
    }

    int saveImage(char ImageName[]) {
        ofstream FCOUT(ImageName);
        if (!FCOUT.is_open())
            return -1;

        FCOUT << "P2\n# This is a comment\n"
            << cols << " " << rows << endl << maxGray << endl;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++)
                FCOUT << ImageData[r][c] << " ";
            FCOUT << endl;
        }
        FCOUT.close();
        imageModified = false;
        return 0;
    }

    void horzontalFlipImage() {
        for (int r = 0; r < rows / 2; r++)
            for (int c = 0; c < cols; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[rows - r - 1][c];
                ImageData[rows - r - 1][c] = T;
            }
        return;
    }

    void verticalFlipImage() {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols / 2; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[r][cols - c - 1];
                ImageData[r][cols - c - 1] = T;
            }
        }
        return;
    }

    void rotateBy90Right() {
        vector<vector<int> > rotated(cols, vector<int>(rows));

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                rotated[j][rows - 1 - i] = ImageData[i][j];
            }
        }
        swap(rows, cols);
        ImageData = rotated;
    }

    void rotateBy90Left() {
        vector<vector<int> > rotated(cols, vector<int>(rows));

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                rotated[cols - 1 - j][i] = ImageData[i][j];
            }
        }

        swap(rows, cols);
        ImageData = rotated;
    }

    void rotateByAngleLeft(double theta) {
        if (rotl == false) {
            unicols = cols;
            unirows = rows;
            rotl = true;
        }

        double rt = (theta * myPI) / 180.0;

        double cosine = cos(rt);
        double sine = sin(rt);

        // calculating the new size of the canvas

        int newWidth = int(round(abs(unicols * cosine) + abs(unirows * sine)));
        int newHeight = int(round(abs(unicols * cosine) + abs(unirows * sine))) + 50;

        vector<vector<int> > rotated(newHeight, vector<int>(newWidth, 0));

        int originalCenterX = cols / 2;
        int originalCenterY = rows / 2;

        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                int x = j - newWidth / 2;
                int y = i - newHeight / 2;

                int newX = round(x * cosine - y * sine) + originalCenterX;
                int newY = round(x * sine + y * cosine) + originalCenterY;

                if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) { // to confirm whether image is within the canvas boundaries
                    rotated[i][j] = ImageData[newY][newX];
                }
            }
        }

        rows = newHeight;
        cols = newWidth;
        ImageData = rotated;
    }

    void rotateByAngleRight(double theta) {
        if (rotr == false) {
            unicols = cols;
            unirows = rows;
            rotr = true;
        }

        double rt = (theta * myPI) / 180.0;

        double cosine = cos(rt);
        double sine = sin(rt);

        // calculating new size of canvas

        int newWidth = int(round(abs(unicols * cosine) + abs(unirows * sine)));
        int newHeight = int(round(abs(unicols * sine) + abs(unirows * cosine))) + 50;

        vector<vector<int> > rotated(newHeight, vector<int>(newWidth, 0));

        int originalCenterX = cols / 2;
        int originalCenterY = rows / 2;

        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                int x = j - newWidth / 2;
                int y = i - newHeight / 2;

                int newX = round(x * cosine + y * sine) + originalCenterX;
                int newY = round(-x * sine + y * cosine) + originalCenterY;

                if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) { // to confirm whether image is within the canvas boundaries
                    rotated[i][j] = ImageData[newY][newX];
                }
            }
        }

        rows = newHeight;
        cols = newWidth;
        ImageData = rotated;
    }

    void changeContrast(double factor) {
        double sum = 0.0;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                sum += ImageData[r][c];
            }
        }
        double avgpix = sum / (rows * cols);

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int calculated = int((ImageData[r][c] - avgpix) * factor + avgpix);

                calculated = max(0, min(maxGray, calculated));

                ImageData[r][c] = calculated;
            }
        }
    }

    void changeSharpness(double factor) {
        vector<vector<int> > og = ImageData;
        int averaged[3][3] = { {1, 1, 1}, {1, 1, 1}, {1, 1, 1} };

        int filterSum = 9;
        double normalizeFactor = 1.0 / filterSum;

        if (factor > 0) {
            vector<vector<int> > og = ImageData;

            int sharpened[3][3] = { {0, -1, 0}, {-1, 5, -1}, {0, -1, 0} };

            for (int r = 1; r < rows - 1; r++) {
                for (int c = 1; c < cols - 1; c++) {
                    int sum = 0;
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            sum += og[r + i][c + j] * sharpened[i + 1][j + 1];
                        }
                    }

                    ImageData[r][c] = int(og[r][c] + factor * sum);
                    ImageData[r][c] = max(0, min(maxGray, ImageData[r][c]));
                }
            }
        }
        else {
            factor = -factor;
            for (int r = 1; r < rows - 1; r++) {
                for (int c = 1; c < cols - 1; c++) {
                    double sum = 0.0;
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            sum += og[r + i][c + j] * averaged[i + 1][j + 1];
                        }
                    }

                    ImageData[r][c] = int(normalizeFactor * sum);
                    ImageData[r][c] = max(0, min(maxGray, ImageData[r][c]));
                }
            }
        }
    }

    void cropImage(int startX, int startY, int cropX, int cropY) {
        if (startX < 0 || startY < 0 || startX + cropX > cols || startY + cropY > rows) {
            cout << "Error: Crop region exceeds image bounds." << endl;
            return;
        }

        vector<vector<int> > cropped(cropY, vector<int>(cropX));

        for (int i = 0; i < cropY; i++) {
            for (int j = 0; j < cropX; j++) {
                cropped[i][j] = ImageData[startY + i][startX + j];
            }
        }

        rows = cropY;
        cols = cropX;
        ImageData = cropped;

        cout << "Image cropped successfully." << endl;
    }

    void scaleUp(double ratio) {
        int newWidth = int(cols * ratio);
        int newHeight = int(rows * ratio);

        vector<vector<int> > resized(newHeight, vector<int>(newWidth));

        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                int originalX = int(j / ratio);
                int originalY = int(i / ratio);

                // Check boundaries
                if (originalX >= 0 && originalX < cols && originalY >= 0 && originalY < rows) {
                    resized[i][j] = ImageData[originalY][originalX];
                }
            }
        }

        rows = newHeight;
        cols = newWidth;
        ImageData = resized;
    }

    void scaleDown(double ratio) {
        int newWidth = int(cols * ratio);
        int newHeight = int(rows * ratio);

        vector<vector<int> > resize(newHeight, vector<int>(newWidth));

        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                int originalX = int(j / ratio);
                int originalY = int(i / ratio);

                if (originalX >= 0 && originalX < cols && originalY >= 0 && originalY < rows) {
                    resize[i][j] = ImageData[originalY][originalX];
                }
            }
        }

        rows = newHeight;
        cols = newWidth;
        ImageData = resize;
    }

    void convertToBinary(int breakpoint) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (ImageData[i][j] < breakpoint) {
                    ImageData[i][j] = 0;
                }
                else {
                    ImageData[i][j] = 255;
                }
            }
        }
    }

    void calculateEdges() {
        vector<vector<int> > edges(ImageData.size(), vector<int>(ImageData[0].size(), 0));

        for (int i = 1; i < ImageData.size() - 1; i++) {
            for (int j = 1; j < ImageData[0].size() - 1; j++) {
                // Calculate central differences for x and y gradients
                int gradientX = (ImageData[i + 1][j] - ImageData[i - 1][j]) / 2;
                int gradientY = (ImageData[i][j + 1] - ImageData[i][j - 1]) / 2;

                // Calculate the edge strength using the magnitude of the gradient
                edges[i][j] = int(sqrt(gradientX * gradientX + gradientY * gradientY));
            }
        }

        ImageData = edges;
    }

    void meanFilter() {
        vector<vector<int> > filtered(ImageData.size(), vector<int>(ImageData[0].size(), 0));

        for (int i = 1; i < ImageData.size() - 1; i++) {
            for (int j = 1; j < ImageData[0].size() - 1; j++) {
                int sum = 0;

                for (int fi = -1; fi <= 1; fi++) {
                    for (int fj = -1; fj <= 1; fj++) {
                        sum += ImageData[i + fi][j + fj];
                    }
                }

                filtered[i][j] = sum / 9; // 9 is the total number of pixel
            }
        }

        // Update the image data
        ImageData = filtered;
    }

    void medianFilter() {
        vector<vector<int> > filtered(ImageData.size(), vector<int>(ImageData[0].size(), 0));

        for (int i = 1; i < ImageData.size() - 1; i++) {
            for (int j = 1; j < ImageData[0].size() - 1; j++) {
                vector<int> adjacent;
                for (int fi = -1; fi <= 1; fi++) {
                    for (int fj = -1; fj <= 1; fj++) {
                        adjacent.push_back(ImageData[i + fi][j + fj]);
                    }
                }

                sort(adjacent.begin(), adjacent.end());

                int medianValue = adjacent[4];

                filtered[i][j] = medianValue;
            }
        }

        ImageData = filtered;
    }

    void combineImages(const Image& secondImage, bool flag) {
        int combinedCols, combinedRows;

        if (flag == true) {
            combinedCols = cols + secondImage.cols;
            combinedRows = max(rows, secondImage.rows);
        }
        else {
            combinedCols = max(cols, secondImage.cols);
            combinedRows = rows + secondImage.rows;
        }

        vector<vector<int> > newImageData(combinedRows, vector<int>(combinedCols, 0));

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                newImageData[i][j] = ImageData[i][j];
            }
        }

        for (int i = 0; i < secondImage.rows; i++) {
            for (int j = 0; j < secondImage.cols; j++) {
                if (flag == true) {
                    newImageData[i][j + cols] = secondImage.ImageData[i][j];
                }
                else {
                    newImageData[i + rows][j] = secondImage.ImageData[i][j];
                }
            }
        }

        cols = combinedCols;
        rows = combinedRows;
        ImageData = newImageData;
    }

    void openImage() {
        if (imageLoaded) {
            string command;

            cout << "Enter 'w' if you are using Windows or 'm' if you are using macOS: ";
            char os;
            cin >> os;

            if (os == 'w' || os == 'W') {
                command = "start D:\\IrfanView\\i_view64.exe " + string(ImageFileName);
            }
            else if (os == 'm' || os == 'M') {
                command = "open " + string(ImageFileName);
            }
            else {
                cout << "You chose an invalid option for operating system. Please try again!" << endl;
                return;
            }
            system(command.c_str());
        }
        else {
            cout << "Error: You did not load or save the image!" << endl;
        }
    }

    void loadLinearFilter(const char filterTxt[]) {
        ifstream currentFilter(filterTxt);
        if (!currentFilter.is_open()) {
            cout << "Error: File not opened" << endl;
            return;
        }

        filter.clear();
        filter.resize(3, vector<double>(3, 0));

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                currentFilter >> filter[i][j];
            }
        }

        currentFilter.close();
    }

    void applyLinearFilter() {
        vector<vector<int> > filtered(rows, vector<int>(cols, 0));

        for (int i = 1; i < rows - 1; i++) {
            for (int j = 1; j < cols - 1; j++) {
                int sum = 0;

                for (int fi = -1; fi <= 1; fi++) {
                    for (int fj = -1; fj <= 1; fj++) {
                        sum += ImageData[i + fi][j + fj] * filter[fi + 1][fj + 1];
                    }
                }

                filtered[i][j] = sum;
            }
        }
        ImageData = filtered;
    }


};

struct Menu {
    vector<string> menuItems;

    Menu(char menuFile[]) {
        loadMenu(menuFile);
    }

    int loadMenu(char menuFile[]) {
        ifstream IN;
        IN.open(menuFile);
        if (!IN.is_open())
            return -1;
        char menuItem[100], TotalItems[10];

        int Choices;

        IN.getline(TotalItems, 8);
        Choices = atoi(TotalItems);
        for (int i = 1; i <= Choices; i++) {
            IN.getline(menuItem, 99);
            menuItems.push_back(menuItem);
        }
        IN.close();
        return Choices;
    }

    int presentMenu() {
        int userChoice;
        int totalChoices = menuItems.size();


        do {
            int k = 1;
            for (int i = 0; i < totalChoices; i++) {
                if (menuItems[i][0] != '*') {
                    cout << k << "\t" << menuItems[i] << endl;
                    k++;
                }
            }
            cout << "Enter Your Choice (1 - " << k - 1 << "): ";
            cin >> userChoice;
        } while (userChoice < 1 || userChoice > totalChoices);
        return userChoice;
    }

};

int main() {
    char MenuFile[] = "MainMenu.txt";
    Image images[2];
    int activeImage = 0;
    int secondAImage = 1;
    int errorCode = 0;
    int userChoice;
    //    int TotalChoices = loadMenu("MainMenu.txt");
    int totalChoices;

    Menu menu(MenuFile);
    totalChoices = menu.menuItems.size();
    do {
        userChoice = menu.presentMenu();
        if (1 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }
        }
        else if (2 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }
        else if (3 == userChoice) {
            images[activeImage].horzontalFlipImage();
            cout << "You need to save the changes " << endl;
        }
        else if (4 == userChoice) {
            images[activeImage].verticalFlipImage();
            cout << "You need to save the changes" << endl;
        }
        else if (5 == userChoice) {
            char j;
            cout << "Enter 0 if you want to decrease the brightness of the image or enter 1 if you want to increase the brightness of the image: ";
            cin >> j;

            if (j == '0') {
                images[activeImage].changeBrightness(0.4);
                cout << "You need to save the changes" << endl;
            }
            else if (j == '1') {
                images[activeImage].changeBrightness(1.6);
                cout << "You need to save the changes" << endl;
            }
        }
        else if (6 == userChoice) {
            int c = -1;
            while (c != 0 && c != 1) {
                cout << "Enter 0 if you want to decrease the contrast or 1 if you want to increase the contrast: ";
                cin >> c;
            }
            if (c == 0) {
                images[activeImage].changeContrast(0.5);
                cout << "You need to save the changes" << endl;
            }
            else if (c == 1) {
                images[activeImage].changeContrast(2);
                cout << "You need to save the changes" << endl;
            }
        }
        else if (7 == userChoice) {
            int c = -1;
            while (c != 0 && c != 1) {
                cout << "Enter 0 if you want to decrease the sharpness or 1 if you want to increase the sharpness: ";
                cin >> c;
            }
            if (c == 0) {
                images[activeImage].changeSharpness(-5);
                cout << "You need to save the changes" << endl;
            }
            else if (c == 1) {
                images[activeImage].changeSharpness(0.1);
                cout << "You need to save the changes" << endl;
            }
        }
        else if (12 == userChoice) {
            images[activeImage].rotateBy90Right();
            cout << "You need to save the changes" << endl;
        }
        else if (13 == userChoice) {
            images[activeImage].rotateBy90Left();
            cout << "You need to save the changes" << endl;
        }
        else if (14 == userChoice) {
            double angle = -1;
            while (angle < 0) {
                cout << "Enter a positive rotation angle (in degrees): ";
                cin >> angle;
            }
            images[activeImage].rotateByAngleRight(angle);
            cout << "You need to save the changes" << endl;
        }
        else if (15 == userChoice) {
            double angle = -1;
            while (angle < 0) {
                cout << "Enter a positive rotation angle (in degrees): ";
                cin >> angle;
            }
            images[activeImage].rotateByAngleLeft(angle);
            cout << "You need to save the changes" << endl;
        }
        else if (8 == userChoice) {
            images[activeImage].cropImage(0, 0, 150, 150);
            cout << "You need to save the changes" << endl;
        }
        else if (9 == userChoice) {
            images[activeImage].scaleUp(2);
            cout << "You need to save the changes" << endl;
        }
        else if (10 == userChoice) {
            images[activeImage].scaleDown(0.5);
            cout << "You need to save the changes" << endl;
        }
        else if (16 == userChoice) {
            images[activeImage].convertToBinary(128);
            cout << "You need to save the changes" << endl;
        }
        else if (17 == userChoice) {
            images[activeImage].calculateEdges();
            cout << "You need to save the changes" << endl;
        }
        else if (18 == userChoice) {
            images[activeImage].meanFilter();
            cout << "You need to save the changes" << endl;
        }
        else if (19 == userChoice) {
            images[activeImage].medianFilter();
            cout << "You need to save the changes" << endl;
        }
        else if (11 == userChoice) {
            char secondImageName[100];
            cout << "Enter the name of the second file: ";
            cin >> secondImageName;

            Image secondImage;
            errorCode = secondImage.loadImage(secondImageName);

            if (errorCode == 0) {
                cout << "The second image was loaded" << endl;
                char g;
                cout << "Enter 0 if you want to combine the images side-by-side or enter 1 if you want to combine the images top-to-bottom: ";
                cin >> g;
                if (g == '0') {
                    images[activeImage].combineImages(secondImage, true);
                    cout << "Images combined successfully" << endl;
                }
                else if (g == '1') {
                    images[activeImage].combineImages(secondImage, false);
                    cout << "Images combined successfully" << endl;
                }
                else {
                    cout << "Invalid choice. You can only choose 0 or 1";
                }
            }
            else {
                cout << "Load Error for the second image: Code " << errorCode << endl;
            }
            cout << "Save your changes" << endl;
        }
        else if (20 == userChoice) {
            images[activeImage].loadLinearFilter("linearfilter.txt");
            images[activeImage].applyLinearFilter();

            cout << "You need to save the changes" << endl;
        }
        else if (21 == userChoice) {
            images[activeImage].openImage();
        }

    } while (userChoice != totalChoices);
    return 0;
}
